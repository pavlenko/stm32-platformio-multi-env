#include <stdint.h>
#include <stdbool.h>

/*
 * USB Device
 * - device descriptor (num configurations, vid/pid)
 *   - configuration descriptor (power, num interfaces, index of string descriptor)
 *     - interface descriptor (class, subclass, protocol)
 *       - endpoint descriptor (setup/in/out, index of string descriptor)
 */

#define USB_DESCRIPTOR_TYPE_DEVICE    0x01
#define USB_DESCRIPTOR_TYPE_CONFIG    0x02
#define USB_DESCRIPTOR_TYPE_STRING    0x03
#define USB_DESCRIPTOR_TYPE_INTERFACE 0x04
#define USB_DESCRIPTOR_TYPE_ENDPOINT  0x05

#define USB_DESCRIPTOR_TYPE_FUNCTIONAL  0x21

const uint8_t usb_device_descriptor[] = {
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

const uint8_t usb_config_descriptor[] = {
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
const uint8_t usb_dfu_runtime_interface_descriptor[] = {
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
const uint8_t usb_dfu_runtime_functional_descriptor[] = {
    0x09, // bLength: Interface Descriptor size = 0x09 - default
    0x21, // bDescriptorType: Descriptor Type = USB_DESCRIPTOR_TYPE_FUNCTIONAL - default
    0x00, // bmAttributes: DFU attributes - depends on implementation
    0x00, // wDetachTimeOut: Wait for DETACH timeout in millis (L byte)
    0x00, // wDetachTimeOut: Wait for DETACH timeout in millis (H byte)
    0x00, // wTransferSize: Maximum number of bytes (L byte)
    0x00, // wTransferSize: Maximum number of bytes (H byte)
    0x00, // bcdDFUVersion: DFU Specification release = 0x1A (L byte of 0x011A)
    0x00, // bcdDFUVersion: DFU Specification release = 0x01 (H byte of 0x011A)
};
// DFU runtime descriptors end

// DFU mode descriptors start
const uint8_t usb_dfu_device_descriptor[] = {
    0x12, // bLength: Device descriptor length = 0x12 - default
    0x01, // bDescriptorType: Descriptor Type = USB_DESCRIPTOR_TYPE_DEVICE - default
    0x00, // bcdUSB: USB Specification Release Number = 0x00 (L byte of 0x0100)
    0x01, // bcdUSB: USB Specification Release Number = 0x01 (H byte of 0x0100)
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

const uint8_t usb_dfu_config_descriptor[] = {
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

int main(void) {
    while (1) {
    }
}
