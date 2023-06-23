#include <stdint.h>
#include "usb_standard.h"

/*
 * USB Device
 * - device descriptor (num configurations, vid/pid)
 *   - configuration descriptor (power, num interfaces, index of string descriptor)
 *     - interface descriptor (class, subclass, protocol)
 *       - endpoint descriptor (setup/in/out, index of string descriptor)
 */
//TODO attribute fields definitions

#define USB_DFU_DESCRIPTOR_TYPE_FUNCTIONAL 0x21u

#define USB_DFU_REQUEST_DETACH 0x00
#define USB_DFU_REQUEST_DNLOAD 0x01
#define USB_DFU_REQUEST_UPLOAD 0x02
#define USB_DFU_REQUEST_GET_STATUS 0x03
#define USB_DFU_REQUEST_CLR_STATUS 0x04
#define USB_DFU_REQUEST_GET_STATE 0x05
#define USB_DFU_REQUEST_ABORT 0x06

enum dfu_state {
	STATE_APP_IDLE,
	STATE_APP_DETACH,
	STATE_DFU_IDLE,
	STATE_DFU_DNLOAD_SYNC,
	STATE_DFU_DNBUSY,
	STATE_DFU_DNLOAD_IDLE,
	STATE_DFU_MANIFEST_SYNC,
	STATE_DFU_MANIFEST,
	STATE_DFU_MANIFEST_WAIT_RESET,
	STATE_DFU_UPLOAD_IDLE,
	STATE_DFU_ERROR,
};

//TODO all descriptors must be packet
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
const uint8_t usb_dfu_device_descriptor[] = {
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

const uint8_t usb_dfu_interface_descriptor[] = {
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

usb_device_t usb;

int main(void) {

    while (1) {
        //_usb_request();
    }
}
