#include <stdint.h>
#include "usb_standard.h"
#include "usb_dfu.h"

/*
 * USB Device
 * - device descriptor (num configurations, vid/pid)
 *   - configuration descriptor (power, num interfaces, index of string descriptor)
 *     - interface descriptor (class, subclass, protocol)
 *       - endpoint descriptor (setup/in/out, index of string descriptor)
 */
//TODO attribute fields definitions

//TODO all descriptors must be packet
const uint8_t __usb_device_descriptor[] = {
    0x12, // bLength: Device descriptor length = 0x12 - default
    0x01, // bDescriptorType: Descriptor Type = USB_DESCRIPTOR_TYPE_DEVICE - default
    0x00, // bcdUSB: USB Specification Release Number = 0x00 (L byte of 0x0200)
    0x02, // bcdUSB: USB Specification Release Number = 0x02 (H byte of 0x0200)
    0x00, // bDeviceClass: Class Code = 0x00 - for define in interfaces
    0x00, // bDeviceSubClass: Subclass code = 0x00 - for define in interfaces
    0x00, // bDeviceProtocol: Protocol code = 0x00 - for define in interfaces
    0x40, // bMaxPacketSize40: Maximum packet size for endpoint zero = 0x40 = 64
    0x83, // idVendor: Vendor ID = (L byte of 0x0483)
    0x04, // idVendor: Vendor ID = (H byte of 0x0483)
    0x50, // idProduct: Product ID = 0x50 (L byte of 0x5750)
    0x57, // idProduct: Product ID = (H byte of 0x5750)
    0x00, // bcdDevice: Device release number = (L byte of 0x0200)
    0x02, // bcdDevice: Device release number = (H byte of 0x0200)
    0x01, // iManufacturer: Index of string descriptor describing manufacturer
    0x02, // iProduct: Index of string descriptor describing product
    0x03, // iSerialNumber: Index of string descriptor describing the device serial number
    0x01, // bNumConfigurations: Number of possible configurations
};

const uint8_t __usb_config_descriptor[] = {
    0x09, // bLength: Configuration Descriptor size = 0x09 - default
    0x02, // bDescriptorType: Descriptor Type = USB_DESCRIPTOR_TYPE_CONFIG - default
    0x00, // wTotalLength: Bytes returned = (L byte) - depends on configuration sub-descriptors
    0x00, // wTotalLength: Bytes returned = (H byte)
    0x02, // bNumInterfaces: Number of interfaces = 2 depends on configuration, > 1 for composite device
    0x01, // bConfigurationValue: Configuration value for use in SetConfiguration()
    0x00, // iConfiguration: Index of string descriptor describing the configuration
    0xC0, // bmAttributes: Configuration characteristics mask = 0xC0 (self-powered) - see specification
    0x32, // bMaxPower: Maximum power consumption = 100 mA - this current is used for detecting VBus
};

// DFU runtime descriptors start
const uint8_t __usb_dfu_runtime_interface_descriptor[] = {
    0x09, // bLength: Interface Descriptor size = 0x09 - default
    0x04, // bDescriptorType: Descriptor Type = USB_DESCRIPTOR_TYPE_INTERFACE - default
    0x01, // bInterfaceNumber: Number of this interface - depends on descriptor position
    0x00, // bAlternateSetting: Alternate setting = 0x00 - must be
    0x00, // bNumEndpoints: Number of endpoints = 0x00 - must be
    0xFE, // bInterfaceClass = Application Specific Class Code - must be = 0xFE
    0x01, // bInterfaceSubClass = Device Firmware Upgrade Code - must be 0x01
    0x01, // bInterfaceProtocol = Runtime protocol - must be 0x01
    0x00, // iInterface: Index of string descriptor for this interface
};

// same for runtime and dfu mode
const uint8_t __usb_dfu_runtime_functional_descriptor[] = {
    0x09, // bLength: Interface Descriptor size = 0x09 - default
    0x21, // bDescriptorType: Descriptor Type = USB_DFU_DESCRIPTOR_TYPE_FUNCTIONAL - default
    0x00, // bmAttributes: DFU attributes - depends on implementation
    0x00, // wDetachTimeOut: Wait for DETACH timeout in millis (L byte)
    0x00, // wDetachTimeOut: Wait for DETACH timeout in millis (H byte)
    0x00, // wTransferSize: Maximum number of bytes (L byte)
    0x00, // wTransferSize: Maxim_usb_requm number of bytes (H byte)
    0x00, // bcdDFUVersion: DFU Specification release = 0x1A (L byte of 0x011A)
    0x00, // bcdDFUVersion: DFU Specification release = 0x01 (H byte of 0x011A)
};
// DFU runtime descriptors end

// DFU mode descriptors start
const uint8_t __usb_dfu_device_descriptor[] = {
    0x12, // bLength: Device descriptor length = 0x12 - default
    0x01, // bDescriptorType: Descriptor Type = USB_DESCRIPTOR_TYPE_DEVICE - default
    0x00, // bcdUSB: USB Specification Release Number = 0x00 (L byte of 0x0100)
    0x02, // bcdUSB: USB Specification Release Number = 0x02 (H byte of 0x0100)
    0x00, // bDeviceClass: Class Code = 0x00 - see interface
    0x00, // bDeviceSubClass: Subclass code = 0x00 - see interface
    0x00, // bDeviceProtocol: Protocol code = 0x00 - see interface
    0x40, // bMaxPacketSize40: Maximum packet size for endpoint zero = 0x40 = 64
    0x83, // idVendor: Vendor ID = (L byte of 0x0483)
    0x04, // idVendor: Vendor ID = (H byte of 0x0483)
    0x50, // idProduct: Product ID = (L byte of 0x5750)
    0x57, // idProduct: Product ID = (H byte of 0x5750)
    0x00, // bcdDevice: Device release number = (L byte of 0x0200)
    0x02, // bcdDevice: Device release number = (H byte of 0x0200)
    0x01, // iManufacturer: Index of string descriptor describing manufacturer
    0x02, // iProduct: Index of string descriptor describing product
    0x03, // iSerialNumber: Index of string descriptor describing the device serial number
    0x01, // bNumConfigurations: Number of possible configurations = 0x01 - must be
};

const uint8_t __usb_dfu_config_descriptor[] = {
    0x09, // bLength: Configuration Descriptor size = 0x09 - default
    0x02, // bDescriptorType: Descriptor Type = USB_DESCRIPTOR_TYPE_CONFIG - default
    0x00, // wTotalLength: Bytes returned = (L byte) - depends on configuration sub-descriptors
    0x00, // wTotalLength: Bytes returned = (H byte)
    0x01, // bNumInterfaces: Number of interfaces = 0x01 - must be
    0x01, // bConfigurationValue: Configuration value for use in SetConfiguration()
    0x00, // iConfiguration: Index of string descriptor describing the configuration
    0xC0, // bmAttributes: Configuration characteristics mask = 0xC0 (self-powered) - see specification
    0x32, // bMaxPower: Maximum power consumption = 100 mA - this current is used for detecting VBus
};

const uint8_t __usb_dfu_interface_descriptor[] = {
    0x09, // bLength: Interface Descriptor size = 0x09 - default
    0x04, // bDescriptorType: Descriptor Type = USB_DESCRIPTOR_TYPE_INTERFACE - default
    0x01, // bInterfaceNumber: Number of this interface = 0x01 - must be
    0x00, // bAlternateSetting: Alternate setting = 0x00 - can be used to access additional memory segments
    0x00, // bNumEndpoints: Number of endpoints = 0x00 - must be
    0xFE, // bInterfaceClass = Application Specific Class Code = 0xFE - must be
    0x01, // bInterfaceSubClass = Device Firmware Upgrade Code = 0x01 - must be
    0x02, // bInterfaceProtocol = DFU mode protocol - must be 0x02
    0x00, // iInterface: Index of string descriptor for this interface
};

// DFU mode VID/PID
#define USB_VID 0x1EAF
#define USB_PID 0x0004

uint8_t buffer[1024];

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

usb_device_t usb_device = {
    .device_descr = &usb_device_descriptor,
    .config_descr = &usb_config_descriptor,
    .strings = usb_strings,
    .num_strings = 4,
    .cb_control = {{.cb = dfu_control_request}},
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
    uint8_t *buf = &buffer;
    uint16_t len = 0;

    usb_request_t req = {};
    while (1) {
        _usb_request(&usb_device, &req, &buf, &len);
    }
}
