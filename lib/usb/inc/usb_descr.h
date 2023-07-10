/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USB_DESCR_H__
#define __USB_DESCR_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

/* Exported types ------------------------------------------------------------*/
/* USB Setup Data structure - Table 9-2 */
typedef struct usb_request_s {
    uint8_t bmRequestType;
    uint8_t bRequest;
    uint16_t wValue;
    uint16_t wIndex;
    uint16_t wLength;
} __attribute__((packed)) usb_request_t;

/* USB Standard Device Descriptor - Table 9-8 */
typedef struct usb_device_descriptor_s {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint16_t bcdUSB;
    uint8_t bDeviceClass;
    uint8_t bDeviceSubClass;
    uint8_t bDeviceProtocol;
    uint8_t bMaxPacketSize0;
    uint16_t idVendor;
    uint16_t idProduct;
    uint16_t bcdDevice;
    uint8_t iManufacturer;
    uint8_t iProduct;
    uint8_t iSerialNumber;
    uint8_t bNumConfigurations;
} __attribute__((packed)) usb_device_descriptor_t;

/* USB Standard Endpoint Descriptor - Table 9-13 */
typedef struct usb_endpoint_descriptor_s {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bEndpointAddress;
    uint8_t bmAttributes;
    uint16_t wMaxPacketSize;
    uint8_t bInterval;

    /* Descriptor ends here.  The following are used internally: */
    const void *extra_ptr;
    uint8_t extra_len;
} __attribute__((packed)) usb_endpoint_descriptor_t;

/* USB Standard Interface Descriptor - Table 9-12 */
typedef struct usb_interface_descriptor_s {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bInterfaceNumber;
    uint8_t bAlternateSetting;
    uint8_t bNumEndpoints;
    uint8_t bInterfaceClass;
    uint8_t bInterfaceSubClass;
    uint8_t bInterfaceProtocol;
    uint8_t iInterface;

    /* Descriptor ends here.  The following are used internally: */
    const void *extra_ptr;
    uint8_t extra_len;
    const usb_endpoint_descriptor_t *endpoint;
} __attribute__((packed)) usb_interface_descriptor_t;

/* From ECN: Interface Association Descriptors, Table 9-Z */
typedef struct usb_interface_assoc_descriptor_s {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bFirstInterface;
    uint8_t bInterfaceCount;
    uint8_t bFunctionClass;
    uint8_t bFunctionSubClass;
    uint8_t bFunctionProtocol;
    uint8_t iFunction;
} __attribute__((packed)) usb_interface_assoc_descriptor_t;

/* USB Standard Interface Descriptor container for altsettings */
typedef struct usb_interface_s {
    const usb_interface_assoc_descriptor_t *association;
    const usb_interface_descriptor_t *altsettings;

    /* Descriptor ends here.  The following are used internally: */
    uint8_t *cur_altsetting;
    uint8_t num_altsetting;
} usb_interface_t;

/* USB Standard Configuration Descriptor - Table 9-10 */
typedef struct usb_config_descriptor_s {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint16_t wTotalLength;
    uint8_t bNumInterfaces;
    uint8_t bConfigurationValue;
    uint8_t iConfiguration;
    uint8_t bmAttributes;
    uint8_t bMaxPower;

    /* Descriptor ends here.  The following are used internally: */
    const usb_interface_t *interfaces;
} __attribute__((packed)) usb_config_descriptor_t;

/* Table 9-15 specifies String Descriptor Zero. */
/* Table 9-16 specified UNICODE String Descriptor. */
typedef struct usb_string_descriptor_s {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint16_t wData[];
} __attribute__((packed)) usb_string_descriptor_t;

/* Exported constants --------------------------------------------------------*/

/* USB Device classes by USB-IF, usage: both|device|interface */
#define USB_CLASS_DEFINED_IN_INTERFACE 0x00 /* device */
#define USB_CLASS_AUDIO                0x01 /* interface */
#define USB_CLASS_COMMUNICATION        0x02 /* both */
#define USB_CLASS_HUMAN_INTERFACE      0x03 /* interface */
#define USB_CLASS_PHYSICAL             0x05 /* interface */
#define USB_CLASS_IMAGE                0x06 /* interface */
#define USB_CLASS_PRINTER              0x07 /* interface */
#define USB_CLASS_MASS_STORAGE         0x08 /* interface */
#define USB_CLASS_HUB                  0x09 /* device */
#define USB_CLASS_CDC_DATA             0x0A /* interface */
#define USB_CLASS_SMART_CARD           0x0B /* interface */
#define USB_CLASS_CONTENT_SECURITY     0x0D /* interface */
#define USB_CLASS_VIDEO                0x0E /* interface */
#define USB_CLASS_PERSONAL_HEALTHCARE  0x0F /* interface */
#define USB_CLASS_AUDIO_VIDEO_DEVICE   0x10 /* interface */
#define USB_CLASS_BILLBOARD_DEVICE     0x11 /* device */
#define USB_CLASS_DIAGNOSTIC_DEVICE    0xDC /* both */
#define USB_CLASS_WIRELESS_CONTROLLER  0x0E /* interface */
#define USB_CLASS_MISCELLANEOUS        0xEF /* both */
#define USB_CLASS_APPLICATION_SPECIFIC 0xFE /* interface*/
#define USB_CLASS_VENDOR_SPECIFIC      0xFF /* both */

/* USB Descriptor Types - Table 9-5 */
#define USB_DESCRIPTOR_TYPE_DEVICE             0x01
#define USB_DESCRIPTOR_TYPE_CONFIG             0x02
#define USB_DESCRIPTOR_TYPE_STRING             0x03
#define USB_DESCRIPTOR_TYPE_INTERFACE          0x04
#define USB_DESCRIPTOR_TYPE_ENDPOINT           0x05
#define USB_DESCRIPTOR_TYPE_DEVICE_QUALIFIER   0x06
#define USB_DESCRIPTOR_TYPE_OTHER_SPEED_CONFIG 0x07
#define USB_DESCRIPTOR_TYPE_INTERFACE_POWER    0x08
#define USB_DESCRIPTOR_TYPE_OTG                0x09
#define USB_DESCRIPTOR_TYPE_DEBUG              0x0A
#define USB_DESCRIPTOR_TYPE_INTERFACE_ASSOC    0x0B

/* USB Descriptor fixed sizes */
#define USB_DESCRIPTOR_SIZE_DEVICE          0x12
#define USB_DESCRIPTOR_SIZE_CONFIG          0x09
#define USB_DESCRIPTOR_SIZE_INTERFACE       0x09
#define USB_DESCRIPTOR_SIZE_INTERFACE_ASSOC 0x08
#define USB_DESCRIPTOR_SIZE_ENDPOINT        0x07

/* USB Configuration Descriptor bmAttributes bit definitions */
#define USB_CONFIG_ATTR_DEFAULT       0x80 /* always required (USB2.0 table 9-10) */
#define USB_CONFIG_ATTR_SELF_POWERED  0x40
#define USB_CONFIG_ATTR_REMOTE_WAKEUP 0x20

/* USB Standard Request Codes - Table 9-4 */
#define USB_REQUEST_GET_STATUS        0x00
#define USB_REQUEST_CLR_FEATURE       0x01
#define USB_REQUEST_SET_FEATURE       0x03
#define USB_REQUEST_SET_ADDRESS       0x05
#define USB_REQUEST_GET_DESCRIPTOR    0x06
#define USB_REQUEST_SET_DESCRIPTOR    0x07
#define USB_REQUEST_GET_CONFIGURATION 0x08
#define USB_REQUEST_SET_CONFIGURATION 0x09
#define USB_REQUEST_GET_INTERFACE     0x0A
#define USB_REQUEST_SET_INTERFACE     0x0B
#define USB_REQUEST_SYNCH_FRAME       0x0C

/* bmRequestType bit definitions */
/* bit 7 : direction */
#define USB_REQ_DIRECTION_MASK 0x80
#define USB_REQ_DIRECTION_IN   0x80
#define USB_REQ_DIRECTION_OUT  0x00
/* bits 6..5 : type */
#define USB_REQ_TYPE_MASK     0x60
#define USB_REQ_TYPE_STANDARD 0x00
#define USB_REQ_TYPE_CLASS    0x20
#define USB_REQ_TYPE_VENDOR      0x40
/* bits 4..0 : recipient */
#define USB_REQ_RECIPIENT_MASK      0x1F
#define USB_REQ_RECIPIENT_DEVICE    0x00
#define USB_REQ_RECIPIENT_INTERFACE 0x01
#define USB_REQ_RECIPIENT_ENDPOINT  0x02
#define USB_REQ_RECIPIENT_OTHER     0x03

/* USB Standard Feature Selectors - Table 9-6 */
#define USB_FEATURE_ENDPOINT_HALT        0x00
#define USB_FEATURE_DEVICE_REMOTE_WAKEUP 0x01
#define USB_FEATURE_TEST_MODE            0x02

/* USB Endpoint Descriptor bmAttributes bit definitions - Table 9-13 */
/* bits 5..4 : usage type (only if ISOCHRONOUS) */
#define USB_ENDPOINT_USAGE_TYPE_MASK     0x30
#define USB_ENDPOINT_USAGE_TYPE_DATA     0x00
#define USB_ENDPOINT_USAGE_TYPE_FEEDBACK 0x10
#define USB_ENDPOINT_USAGE_TYPE_IMPLICIT 0x20
/* bits 3..2 : Sync type (only if ISOCHRONOUS) */
#define USB_ENDPOINT_SYNC_TYPE_MASK         0x0C
#define USB_ENDPOINT_SYNC_TYPE_NOSYNCRONOUS 0x00
#define USB_ENDPOINT_SYNC_TYPE_ASYNC        0x04
#define USB_ENDPOINT_SYNC_TYPE_ADAPTIVE     0x08
#define USB_ENDPOINT_SYNC_TYPE_SYNCRONOUS   0x0C
/* bits 1..0 : transfer type */
#define USB_ENDPOINT_TRANSFER_TYPE_MASK        0x03
#define USB_ENDPOINT_TRANSFER_TYPE_CONTROL     0x00
#define USB_ENDPOINT_TRANSFER_TYPE_ISOCHRONOUS 0x01
#define USB_ENDPOINT_TRANSFER_TYPE_BULK        0x02
#define USB_ENDPOINT_TRANSFER_TYPE_INTERRUPT   0x03

/* 16-bit Language ID for 0 string descriptor */
#define USB_LANGID_ENGLISH_US 0x0409

/* Exported macro ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Exported functions prototypes ---------------------------------------------*/
/* Private defines -----------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif // __USB_DESCR_H__