#include <stdint.h>

#include "usb_control.h"
#include "usb_standard.h"
#include "usb_dfu.h"

dfu_functional_descriptor_t dfu_functional_descriptor = {
    .bLength         = sizeof(dfu_functional_descriptor_t),
    .bDescriptorType = DFU_DESCRIPTOR_TYPE_FUNCTIONAL,
    .bmAttributes    = DFU_ATTR_CAN_UPLOAD | DFU_ATTR_CAN_DOWNLOAD | DFU_ATTR_WILL_DETACH,
    .wDetachTimeout  = 0xFF,
    .wTransferSize   = 0x0400, // 0x0400 = 1024
    .bcdDFUVersion   = 0x011A,
};

usb_interface_descriptor_t dfu_interface_descriptor = {
    .bLength            = USB_DESCRIPTOR_SIZE_INTERFACE,
    .bDescriptorType    = USB_DESCRIPTOR_TYPE_INTERFACE,
    .bInterfaceNumber   = 0x00,
    .bAlternateSetting  = 0x00,
    .bNumEndpoints      = 0x00,
    .bInterfaceClass    = 0xFE, // DFU Class
    .bInterfaceSubClass = 0x01,
    .bInterfaceProtocol = 0x02,
    .iInterface         = 0x04,

    .extra_ptr = &dfu_functional_descriptor,
    .extra_len = sizeof(dfu_functional_descriptor),
};

usb_interface_t interfaces[] = {
    {.altsettings = &dfu_interface_descriptor, .num_altsetting = 1}
};

usb_config_descriptor_t usb_config_descriptor = {
    .bLength             = USB_DESCRIPTOR_SIZE_CONFIG,
    .bDescriptorType     = USB_DESCRIPTOR_TYPE_CONFIG,
    .wTotalLength        = 0x00,// calculated on fly
    .bNumInterfaces      = 0x01,
    .bConfigurationValue = 0x01,
    .iConfiguration      = 0x00,
    .bmAttributes        = USB_CONFIG_ATTR_DEFAULT | USB_CONFIG_ATTR_SELF_POWERED,
    .bMaxPower           = 0x32,// 0x32 = 50, milliampers

    .interfaces = interfaces,
};

usb_device_descriptor_t usb_device_descriptor = {
    .bLength            = USB_DESCRIPTOR_SIZE_DEVICE,
    .bDescriptorType    = USB_DESCRIPTOR_TYPE_DEVICE,
    .bcdUSB             = 0x0200,
    .bDeviceClass       = 0x00,
    .bDeviceSubClass    = 0x00,
    .bDeviceProtocol    = 0x00,
    .bMaxPacketSize0    = 0x40,
    .idVendor           = USB_VID,
    .idProduct          = USB_PID,
    .bcdDevice          = 0x0200,
    .iManufacturer      = 0x01,
    .iProduct           = 0x02,
    .iSerialNumber      = 0x03,
    .bNumConfigurations = 0x01,
};

static const char *usb_strings[] = {
    "MASTER",
    "HexaPod Mark II Bootloader",
    "HexaPod Mark II",
    "@Internal Flash   /0x08000000/8*001Ka,56*001Kg",
};

uint8_t usb_buffer[1024];
uint8_t dfu_buffer[1024];

//TODO create init function
dfu_t dfu = {
    .buf   = dfu_buffer,
    .state = DFU_STATE_DFU_IDLE,
    .descr = &dfu_functional_descriptor,
};

//TODO create init function
usb_device_t usb_device = {
    .device_descr = &usb_device_descriptor,
    .config_descr = &usb_config_descriptor,
    .strings = usb_strings,
    .num_strings = 4,
    .cb_control = {
        {.cb = dfu_cb_control, .ptr = &dfu},
    },
};

void dfu_system_reset()
{}

void dfu_memory_unlock()
{}

void dfu_memory_lock()
{}

void dfu_memory_erase_page(uint32_t address)
{
    (void) address;
}

void dfu_memory_write_uint16(uint32_t address, uint16_t data)
{
    (void) address;
    (void) data;
}

int main(void) {
    usb_request_t req = {};
    while (1) {
        usb_control_request_dispatch(&usb_device, &req);
    }
}

void usb_ep_setup(usb_device_t *dev, uint8_t address, uint8_t type, uint16_t max_size, usb_cb_endpoint cb)
{
    (void) dev;
    (void) address;
    (void) type;
    (void) max_size;
    (void) cb;
}

void usb_ep_reset(usb_device_t *dev)
{
    (void) dev;
}

uint16_t usb_ep_read_packet(usb_device_t *dev, uint8_t address, const void *buf, uint16_t len)
{
    (void) dev;
    (void) address;
    (void) buf;
    (void) len;
    return 0;
}

void usb_ep_write_packet(usb_device_t *dev, uint8_t address, const void *buf, uint16_t len)
{
    (void) dev;
    (void) address;
    (void) buf;
    (void) len;
}

void usb_ep_stall_set(usb_device_t *dev, uint8_t address, uint8_t stall)
{
    (void) dev;
    (void) address;
    (void) stall;
}

uint8_t usb_ep_stall_get(usb_device_t *dev, uint8_t address)
{
    (void) dev;
    (void) address;
    return 0;
}

void usb_ep_nak_set(usb_device_t *dev, uint8_t address, uint8_t nak)
{
    (void) dev;
    (void) address;
    (void) nak;
}
