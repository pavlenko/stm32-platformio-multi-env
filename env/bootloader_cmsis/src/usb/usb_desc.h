#ifndef __USB_DESC_H
#define __USB_DESC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"

typedef struct {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint16_t wData[];
} __attribute__((packed)) usb_string_descriptor_t;

typedef struct {
    uint8_t bLength;            // Size of this descriptor in bytes.
    uint8_t bDescriptorType;    // Device Descriptor Type = 1.
    uint16_t bcdUSB;            // USB Specification Release Number in Binary-Coded Decimal (i.e., 2.10 is 210h).
    uint8_t bDeviceClass;       // Class code (assigned by the USB-IF).
    uint8_t bDeviceSubClass;    // Subclass code (assigned by the USB-IF).
    uint8_t bDeviceProtocol;    // Protocol code (assigned by the USB-IF).
    uint8_t bMaxPacketSize0;    // Maximum packet size for Endpoint zero (only 8, 16, 32, or 64 are valid).
    uint16_t idVendor;          // Vendor ID (assigned by the USB-IF).
    uint16_t idProduct;         // Product ID (assigned by the manufacturer).
    uint16_t bcdDevice;         // Device release number in binary-coded decimal.
    uint8_t	iManufacturer;      // Index of string descriptor describing manufacturer.
    uint8_t	iProduct;           // Index of string descriptor describing product.
    uint8_t	iSerialNumber;      // Index of string descriptor describing the device's serial number.
    uint8_t bNumConfigurations; // Number of possible configurations.
} __attribute__((packed)) usb_device_descriptor_t;

typedef struct {
    uint8_t bLength;             // Size of this descriptor in bytes.
    uint8_t	bDescriptorType;     // Configuration Descriptor Type = 2.
    uint16_t wTotalLength;       // Total length of data returned for this configuration.
    uint8_t	bNumInterfaces;      // Number of interfaces supported by this configuration.
    uint8_t	bConfigurationValue; // Value to select this configuration with SetConfiguration().
    uint8_t	iConfiguration;      // Index of string descriptor describing this configuration.
    uint8_t	bmAttributes;        // Configuration characteristics
    uint8_t bMaxPower;           // Maximum power consumption of the USB device.
} __attribute__((packed)) usb_config_descriptor_t;

typedef struct {
    uint8_t bLength;            // Size of this descriptor in bytes.
    uint8_t bDescriptorType;    // Interface Descriptor Type = 4.
    uint8_t bInterfaceNumber;   // The number of this interface.
    uint8_t	bAlternateSetting;  // Value used to select an alternate setting.
    uint8_t	bNumEndpoints;      // Number of endpoints used by this interface (excluding endpoint zero).
    uint8_t	bInterfaceClass;    // Class code (assigned by the USB-IF).
    uint8_t	bInterfaceSubClass; // Subclass code (assigned by the USB-IF).
    uint8_t	bInterfaceProtocol; // Protocol code (assigned by the USB).
    uint8_t	iInterface;         // Index of string descriptor describing this interface.
} __attribute__((packed)) usb_interface_descriptor_t;

typedef struct {
    uint8_t bLength;          // Size of this descriptor in bytes.
    uint8_t bDescriptorType;  // Endpoint Descriptor Type = 5.
    uint8_t bEndpointAddress; // The address of the endpoint on the USB device described by this descriptor.
    uint8_t	bmAttributes;     // The endpoint attribute when configured through bConfigurationValue.
    uint16_t wMaxPacketSize;  // Is the maximum packet size of this endpoint.
    uint8_t	bInterval;        // Interval for polling endpoint for data transfers.
} usb_endpoint_descriptor_t;

#ifdef __cplusplus
}
#endif

#endif //__USB_DESC_H
