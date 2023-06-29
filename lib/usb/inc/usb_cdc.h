/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USB_CDC_H__
#define __USB_CDC_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Abbreviations
 * 
 * CDC  - Communication Device Class
 * ATM  - Asynchronous Transfer Mode
 * ECM  - Ethernet Control Model
 * ISDN - Integrated Services Digital Network
 * PSTN - Public Switched Telephone Network
 */

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

/* Private includes ----------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/

/* CDC PSTN Table 17: Line Coding enums */
enum cdc_line_coding_bCharFormat {
	CDC_STOP_BITS_1   = 0,
	CDC_STOP_BITS_1_5 = 1,
	CDC_STOP_BITS_2   = 2,
};

enum cdc_line_coding_bParityType {
	CDC_PARITY_NO    = 0,
	CDC_PARITY_ODD   = 1,
	CDC_PARITY_EVEN  = 2,
	CDC_PARITY_MARK  = 3,
	CDC_PARITY_SPACE = 4,
};

enum cdc_line_coding_bDataBits {
	CDC_DATA_BITS_5  = 5,
	CDC_DATA_BITS_6  = 6,
	CDC_DATA_BITS_7  = 7,
	CDC_DATA_BITS_8  = 8,
	CDC_DATA_BITS_16 = 16,
};

/* CDC 6.3: Management Element Notifications */
typedef struct cdc_notification_s {
	uint8_t bmRequestType;
	uint8_t bNotificationCode;
	uint16_t wValue;
	uint16_t wIndex;
	uint16_t wLength;
} __attribute__((packed)) cdc_notification_t;

/* CDC Table 15: Class-Specific Descriptor Header Format */
typedef struct cdc_header_functional_descriptor_s {
	uint8_t bFunctionLength;
	uint8_t bDescriptorType;
	uint8_t bDescriptorSubtype;
	uint16_t bcdCDC;
} __attribute__((packed)) cdc_header_functional_descriptor_t;

/* CDC Table 16: Union Interface Functional Descriptor */
typedef struct cdc_union_functional_descriptor_s {
	uint8_t bFunctionLength;
	uint8_t bDescriptorType;
	uint8_t bDescriptorSubtype;
	uint8_t bControlInterface;
	uint8_t bSubordinateInterface0;
	/* ... */
	/* bSubordinateInterfaceN-1 */
} __attribute__((packed)) cdc_union_functional_descriptor_t;

/* CDC Table 17: Country Selection Functional Descriptor */
typedef struct cdc_country_selection_functional_descriptor_s {
	uint8_t bFunctionLength;
	uint8_t bDescriptorType;
	uint8_t bDescriptorSubtype;
	uint8_t iCountryCodeRelDate;
	uint16_t wCountryCode0;
	/* ... */
	/* wCountryCode0-1 */
} __attribute__((packed)) cdc_country_selection_functional_descriptor_t;

/* CDC PSTN Table 3: Call Management Functional Descriptor */
typedef struct cdc_call_management_functional_descriptor_s {
	uint8_t bFunctionLength;
	uint8_t bDescriptorType;
	uint8_t bDescriptorSubtype;
	uint8_t bmCapabilities;//TODO bits 
	uint8_t bDataInterface;
} __attribute__((packed)) cdc_call_management_functional_descriptor_t;

/* CDC PSTN Table 4: Abstract Control Management Functional Descriptor */
typedef struct cdc_acm_functional_descriptor_s {
	uint8_t bFunctionLength;
	uint8_t bDescriptorType;
	uint8_t bDescriptorSubtype;
	uint8_t bmCapabilities;
} __attribute__((packed)) cdc_acm_functional_descriptor_t;

/* CDC PSTN Table 17: Line Coding Structure */
typedef struct cdc_line_coding_s {
	uint32_t dwDTERate;
	uint8_t bCharFormat;
	uint8_t bParityType;
	uint8_t bDataBits;
} __attribute__((packed)) cdc_line_coding_t;

/* Exported constants --------------------------------------------------------*/

/* CDC Table 4: Communications Class Subclass Codes */
#define CDC_CC_SUBCLASS_DLCM 0x01
#define CDC_CC_SUBCLASS_ACM  0x02
#define CDC_CC_SUBCLASS_TCM  0x03
#define CDC_CC_SUBCLASS_MCCM 0x04
#define CDC_CC_SUBCLASS_CAPI 0x05
#define CDC_CC_SUBCLASS_ENCM 0x06
#define CDC_CC_SUBCLASS_ATM  0x07
#define CDC_CC_SUBCLASS_WHCM 0x08
#define CDC_CC_SUBCLASS_DM   0x09
#define CDC_CC_SUBCLASS_DM   0x09
#define CDC_CC_SUBCLASS_MDLM 0x0A
#define CDC_CC_SUBCLASS_OBEX 0x0B
#define CDC_CC_SUBCLASS_EEM  0x0C
#define CDC_CC_SUBCLASS_NCM  0x0D

/* CDC Table 5 Communications Class Protocol Codes */
#define CDC_CC_PROTOCOL_NONE     0x00
#define CDC_CC_PROTOCOL_AT       0x01
/* Unused 0x02 ... 0x07 */
/* Reserved 0x08 ... 0xFD */
/* Unused 0xFE ... 0xFF */

/* CDC Table 6: Data Class Interface Codes */
#define CDC_DC_CLASS_DATA 0x0A

/* CDC Table 12: Type Values for the bDescriptorType Field */
#define CDC_DESCRIPTOR_TYPE_CS_INTERFACE 0x24
#define CDC_DESCRIPTOR_TYPE_CS_ENDPOINT  0x25

/* CDC Table 13: bDescriptorSubType in Communications Class Functional Descriptors */
#define CDC_DESCRIPTOR_SUBTYPE_HEADER          0x00
#define CDC_DESCRIPTOR_SUBTYPE_CALL_MANAGEMENT 0x01
#define CDC_DESCRIPTOR_SUBTYPE_ACM             0x02
/* Unused 0x03 ... 0x05 */
#define CDC_DESCRIPTOR_SUBTYPE_UNION           0x06
/* Unused 0x07 ... 0x1A */
/* Reserved 0x1B ... 0xFF */

/* CDC PSTN Table 3: bmCapabilities bits 1...0 */
#define CDC_CM_CAPAB_MASK            0x03
#define CDC_CM_CAPAB_OVER_DATA_CLASS 0x02
#define CDC_CM_CAPAB_HANDLE_ITSELF   0x01

/* CDC PSTN Table 4: bmCapabilities bits 3...0 */
#define CDC_ACM_CAPAB_MASK                 0x0F
#define CDC_ACM_CAPAB_SUPPORT_FEATURE      0x01
#define CDC_ACM_CAPAB_SUPPORT_STATE_CODING 0x02
#define CDC_ACM_CAPAB_SUPPORT_BREAK        0x04
#define CDC_ACM_CAPAB_SUPPORT_NETWORK      0x08

/* CDC PSTN Table 30: Class-Specific Notification Codes for PSTN subclasses */
#define CDC_NOTIFY_NETWORK_CONNECTION  0x00
#define CDC_NOTIFY_RESPONSE_AVAILABLE  0x01
#define CDC_NOTIFY_AUX_JACK_HOOK_STATE 0x08
#define CDC_NOTIFY_RING_DETECT         0x09
#define CDC_NOTIFY_SERIAL_STATE        0x20
#define CDC_NOTIFY_CALL_STATE_CHANGE   0x28
#define CDC_NOTIFY_LINE_STATE_CHANGE   0x23

/* Exported macro ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif // __USB_CDC_H__