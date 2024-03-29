#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/flash.h>
#include <libopencm3/cm3/scb.h>
#include <libopencm3/usb/usbd.h>
#include <libopencm3/usb/dfu.h>

#include "config.h"
//TODO optimize to fit 4kb
#define APP_ADDRESS (FLASH_BASE + DFU_SIZE)

/* Commands sent with wBlockNum == 0 as per ST implementation. */
#define CMD_SETADDR    0x21
#define CMD_ERASE    0x41

/* We need a special large control buffer for this device: */
uint8_t usbd_control_buffer[DFU_TRANSFER_SIZE];

static enum dfu_state usbdfu_state = STATE_DFU_IDLE;

static struct {
    uint8_t buf[sizeof(usbd_control_buffer)];
    uint16_t len;
    uint32_t addr;
    uint16_t blocknum;
} prog;

const struct usb_device_descriptor dev = {
        .bLength = USB_DT_DEVICE_SIZE,
        .bDescriptorType = USB_DT_DEVICE,
        .bcdUSB = 0x0200,
        .bDeviceClass = 0,
        .bDeviceSubClass = 0,
        .bDeviceProtocol = 0,
        .bMaxPacketSize0 = 64,
        .idVendor = USB_VID,
        .idProduct = USB_PID,
        .bcdDevice = 0x0200,
        .iManufacturer = 1,
        .iProduct = 2,
        .iSerialNumber = 3,
        .bNumConfigurations = 1,
};

const struct usb_dfu_descriptor dfu_function = {
        .bLength = sizeof(struct usb_dfu_descriptor),
        .bDescriptorType = DFU_FUNCTIONAL,
        .bmAttributes = USB_DFU_CAN_UPLOAD | USB_DFU_CAN_DOWNLOAD | USB_DFU_WILL_DETACH,
        .wDetachTimeout = 255,
        .wTransferSize = DFU_TRANSFER_SIZE,
        .bcdDFUVersion = 0x011A,
};

const struct usb_interface_descriptor iface = {
        .bLength = USB_DT_INTERFACE_SIZE,
        .bDescriptorType = USB_DT_INTERFACE,
        .bInterfaceNumber = 0,
        .bAlternateSetting = 0,
        .bNumEndpoints = 0,
        .bInterfaceClass = 0xFE, /* Device Firmware Upgrade */
        .bInterfaceSubClass = 1,
        .bInterfaceProtocol = 2,

        /* The ST Microelectronics DfuSe application needs this string.
         * The format isn't documented... */
        .iInterface = 4,

        .extra = &dfu_function,
        .extralen = sizeof(dfu_function),
};

const struct usb_interface ifaces[] = {{
                                               .num_altsetting = 1,
                                               .altsetting = &iface,
                                       }};

const struct usb_config_descriptor config = {
        .bLength = USB_DT_CONFIGURATION_SIZE,
        .bDescriptorType = USB_DT_CONFIGURATION,
        .wTotalLength = 0,
        .bNumInterfaces = 1,
        .bConfigurationValue = 1,
        .iConfiguration = 0,
        .bmAttributes = 0xC0,
        .bMaxPower = 0x32,

        .interface = ifaces,
};

static const char *usb_strings[] = {
        USB_VENDOR_STRING,
        USB_PRODUCT_STRING,
        USB_PRODUCT_SERIAL_NUM,
        /* This string is used by ST Microelectronics' DfuSe utility. */
        "@Internal Flash   /0x08000000/8*001Ka,56*001Kg",
};

static uint8_t usbdfu_getstatus(usbd_device *usbd_dev, uint32_t *bwPollTimeout)
{
    (void)usbd_dev;

    switch (usbdfu_state) {
        case STATE_DFU_DNLOAD_SYNC:
            usbdfu_state = STATE_DFU_DNBUSY;
            *bwPollTimeout = 100;
            return DFU_STATUS_OK;
        case STATE_DFU_MANIFEST_SYNC:
            /* Device will reset when read is complete. */
            usbdfu_state = STATE_DFU_MANIFEST;
            return DFU_STATUS_OK;
        default:
            return DFU_STATUS_OK;
    }
}

static void usbdfu_getstatus_complete(usbd_device *usbd_dev, struct usb_setup_data *req)
{
    int i;
    (void)req;
    (void)usbd_dev;

    switch (usbdfu_state) {
        case STATE_DFU_DNBUSY:
            flash_unlock();
            if (prog.blocknum == 0) {
                switch (prog.buf[0]) {
                    case CMD_ERASE:
                    {
                        uint32_t *dat = (uint32_t *)(prog.buf + 1);
                        flash_erase_page(*dat);
                    }
                        break;
                    case CMD_SETADDR:
                    {
                        uint32_t *dat = (uint32_t *)(prog.buf + 1);
                        prog.addr = *dat;
                    }
                        break;
                }
            } else {
                uint32_t baseaddr = prog.addr + ((prog.blocknum - 2) *
                                                 dfu_function.wTransferSize);
                for (i = 0; i < prog.len; i += 2) {
                    uint16_t *dat = (uint16_t *)(prog.buf + i);
                    flash_program_half_word(baseaddr + i, *dat);
                }
            }
            flash_lock();

            /* Jump straight to dfuDNLOAD-IDLE, skipping dfuDNLOAD-SYNC. */
            usbdfu_state = STATE_DFU_DNLOAD_IDLE;
            return;
        case STATE_DFU_MANIFEST:
            /* USB device must detach, we just reset... */
            scb_reset_system();
            return; /* Will never return. */
        default:
            return;
    }
}

static enum usbd_request_return_codes usbdfu_control_request(
        usbd_device *usbd_dev,
        struct usb_setup_data *req,
        uint8_t **buf,
        uint16_t *len,
        void (**complete)(usbd_device *usbd_dev, struct usb_setup_data *req)
) {
    if ((req->bmRequestType & 0x7F) != 0x21)
        return USBD_REQ_NOTSUPP; /* Only accept class request. */

    switch (req->bRequest) {
        case DFU_DNLOAD:
            if ((len == NULL) || (*len == 0)) {
                usbdfu_state = STATE_DFU_MANIFEST_SYNC;
            } else {
                /* Copy download data for use on GET_STATUS. */
                prog.blocknum = req->wValue;
                prog.len = *len;
                memcpy(prog.buf, *buf, *len);
                usbdfu_state = STATE_DFU_DNLOAD_SYNC;
            }
            return USBD_REQ_HANDLED;
        case DFU_CLRSTATUS:
            /* Clear error and return to dfuIDLE. */
            if (usbdfu_state == STATE_DFU_ERROR)
                usbdfu_state = STATE_DFU_IDLE;
            return USBD_REQ_HANDLED;
        case DFU_ABORT:
            /* Abort returns to dfuIDLE state. */
            usbdfu_state = STATE_DFU_IDLE;
            return USBD_REQ_HANDLED;
        case DFU_DETACH:
            usbdfu_state = STATE_DFU_MANIFEST_SYNC;
            *complete = usbdfu_getstatus_complete;
            return USBD_REQ_HANDLED;
        case DFU_UPLOAD:
            /* Upload not supported for now. */
            return USBD_REQ_NOTSUPP;
        case DFU_GETSTATUS: {
            uint32_t bwPollTimeout = 0; /* 24-bit integer in DFU class spec */
            (*buf)[0] = usbdfu_getstatus(usbd_dev, &bwPollTimeout);
            (*buf)[1] = bwPollTimeout & 0xFF;
            (*buf)[2] = (bwPollTimeout >> 8) & 0xFF;
            (*buf)[3] = (bwPollTimeout >> 16) & 0xFF;
            (*buf)[4] = usbdfu_state;
            (*buf)[5] = 0; /* iString not used here */
            *len = 6;
            *complete = usbdfu_getstatus_complete;
            return USBD_REQ_HANDLED;
        }
        case DFU_GETSTATE:
            /* Return state with no state transision. */
            *buf[0] = usbdfu_state;
            *len = 1;
            return USBD_REQ_HANDLED;
    }

    return USBD_REQ_NOTSUPP;
}

static void usbdfu_set_config(usbd_device *usbd_dev, uint16_t wValue)
{
    (void)wValue;

    usbd_register_control_callback(
        usbd_dev,
        USB_REQ_TYPE_CLASS | USB_REQ_TYPE_INTERFACE,
        USB_REQ_TYPE_TYPE | USB_REQ_TYPE_RECIPIENT,
        usbdfu_control_request
    );
}

int main(void) {
    usbd_device *usbd_dev;

    // Setup GPIO
    rcc_periph_clock_enable(DFU_CHECK_GPIO_RCC);
    gpio_set_mode(DFU_CHECK_GPIO_PORT, GPIO_MODE_INPUT, GPIO_CNF_INPUT_FLOAT, DFU_CHECK_GPIO_PIN);
    gpio_clear(DFU_CHECK_GPIO_PORT, DFU_CHECK_GPIO_PIN);
    for (uint16_t i = 0; i < 512; i++) {
        __asm__("nop");
    }

    // Check GPIO
    if (!gpio_get(DFU_CHECK_GPIO_PORT, DFU_CHECK_GPIO_PIN)) {
        // Boot the application if it's valid.
        if ((*(volatile uint32_t *)APP_ADDRESS & 0x2FFE0000) == 0x20000000) {
            // Set vector table base address.
            SCB_VTOR = APP_ADDRESS & 0xFFFF;
            // Initialise master stack pointer.
            __asm__ volatile("msr msp, %0"::"g" (*(volatile uint32_t *)APP_ADDRESS));
            // Jump to application.
            (*(void (**)(void))(APP_ADDRESS + 4))();
        }
    }

    // Setup bootloader
    rcc_clock_setup_in_hse_8mhz_out_72mhz();

    rcc_periph_clock_enable(RCC_GPIOC);

    // Configure on-board led (PC13, no pull)
    gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_OPENDRAIN, GPIO13);
    gpio_clear(GPIOC, GPIO13); // turn on for check
    for (uint16_t i = 0; i < 65535; i++) {
        __asm__("nop");
    }

    usbd_dev = usbd_init(&st_usbfs_v1_usb_driver, &dev, &config, usb_strings, 4, usbd_control_buffer, sizeof(usbd_control_buffer));
    usbd_register_set_config_callback(usbd_dev, usbdfu_set_config);

    gpio_set(GPIOC, GPIO13);// turn of: init success

    while (1) {
        usbd_poll(usbd_dev);
    }
}
