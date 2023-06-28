/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USB_CDC_H__
#define __USB_CDC_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

/* Private includes ----------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/

/* Table 15: Class-Specific Descriptor Header Format */
typedef struct usb_cdc_header_descriptor_s {
	uint8_t bFunctionLength;
	uint8_t bDescriptorType;
	uint8_t bDescriptorSubtype;
	uint16_t bcdCDC;
} __attribute__((packed)) usb_cdc_header_descriptor_t;

/* Table 16: Union Interface Functional Descriptor */
typedef struct usb_cdc_union_descriptor_s {
	uint8_t bFunctionLength;
	uint8_t bDescriptorType;
	uint8_t bDescriptorSubtype;
	uint8_t bControlInterface;
	uint8_t bSubordinateInterface0;
	/* ... */
} __attribute__((packed)) usb_cdc_union_descriptor_t;

/**
 * Definitions for Abstract Control Model devices from:
 * "Universal Serial Bus Communications Class Subclass Specification for
 * PSTN Devices"
 */

/* Table 3: Call Management Functional Descriptor */
typedef struct usb_cdc_call_management_descriptor {
	uint8_t bFunctionLength;
	uint8_t bDescriptorType;
	uint8_t bDescriptorSubtype;
	uint8_t bmCapabilities;
	uint8_t bDataInterface;
} __attribute__((packed)) usb_cdc_call_management_descriptor_t;

/* Table 4: Abstract Control Management Functional Descriptor */
typedef struct usb_cdc_acm_descriptor {
	uint8_t bFunctionLength;
	uint8_t bDescriptorType;
	uint8_t bDescriptorSubtype;
	uint8_t bmCapabilities;
} __attribute__((packed)) usb_cdc_acm_descriptor_t;

/* Exported constants --------------------------------------------------------*/

/* Table 4: Class Subclass Code */
#define CDC_SUBCLASS_DLCM 0x01
#define CDC_SUBCLASS_ACM  0x02

/* Table 5 Communications Interface Class Control Protocol Codes */
#define CDC_PROTOCOL_NONE 0x00
#define CDC_PROTOCOL_AT   0x01

/* Table 6: Data Interface Class Code */
#define USB_CLASS_DATA 0x0A

/* Table 12: Type Values for the bDescriptorType Field */
#define CS_INTERFACE 0x24
#define CS_ENDPOINT  0x25

/* Table 13: bDescriptor SubType in Communications Class Functional Descriptors */
#define USB_CDC_TYPE_HEADER          0x00
#define USB_CDC_TYPE_CALL_MANAGEMENT 0x01
#define USB_CDC_TYPE_ACM             0x02
#define USB_CDC_TYPE_UNION           0x06

/* Exported macro ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif // __USB_CDC_H__