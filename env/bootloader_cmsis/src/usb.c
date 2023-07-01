#include "usb.h"

/* Includes ------------------------------------------------------------------*/
#include <usb_dfu.h>

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* External function prototypes ----------------------------------------------*/
/* External variables --------------------------------------------------------*/
/* Function definitions ------------------------------------------------------*/

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
    .bInterfaceClass    = USB_CLASS_APPLICATION_SPECIFIC, // DFU Class
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

const char *usb_strings[] = {
    "MASTER",
    "HexaPod Mark II Bootloader",
    "HexaPod Mark II",
    "@Internal Flash   /0x08000000/8*001Ka,56*001Kg",
};