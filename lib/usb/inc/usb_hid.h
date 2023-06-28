/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USB_HID_H__
#define __USB_HID_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

/* Private includes ----------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/

//TODO how to define report descriptors
typedef struct hid_functional_descriptor_s {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint16_t bcdHID;
    uint8_t bCountryCode;
    uint8_t bNumDescriptors;

    /* Descriptor ends here.  The following are used internally: */
    const void *extra_ptr;
    uint8_t extra_len;
} __attribute__((packed)) hid_functional_descriptor_t;

/* Exported constants --------------------------------------------------------*/

/* HID Interface Descriptor bInterfaceSubClass (USB HID 4.2) */
#define HID_INTERFACE_SUBCLASS_NONE 0x00
#define HID_INTERFACE_SUBCLASS_BOOT 0x01

/* HID Interface Descriptor bInterfaceProtocol (USB HID 4.3) */
#define HID_INTERFACE_PROTOCOL_NONE     0x00
#define HID_INTERFACE_PROTOCOL_KEYBOARD 0x01
#define HID_INTERFACE_PROTOCOL_MOUSE    0x02

/* HID Function Descriptor Types (USB HID 7.1) */
#define HID_DESCRIPTOR_TYPE_HID      0x21
#define HID_DESCRIPTOR_TYPE_REPORT   0x22
#define HID_DESCRIPTOR_TYPE_PHYSICAL 0x23

/* HID Requests (USB HID 7.2) */
#define HID_REQUEST_GET_REPORT   0x01
#define HID_REQUEST_GET_IDLE     0x02
#define HID_REQUEST_GET_PROTOCOL 0x03
#define HID_REQUEST_SET_REPORT   0x09
#define HID_REQUEST_SET_IDLE     0x0A
#define HID_REQUEST_SET_PROTOCOL 0x0B

/* HID Report Types (USB HID 7.2.1) */
#define HID_REPORT_TYPE_INPUT   0x01
#define HID_REPORT_TYPE_OUTPUT  0x02
#define HID_REPORT_TYPE_FEATURE 0x03

/* HID Protocols (USB HID 7.2.5) */
#define HID_PROTOCOL_BOOT   0x00
#define HID_PROTOCOL_REPORT 0x01

/* Exported macro ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif // __USB_HID_H__