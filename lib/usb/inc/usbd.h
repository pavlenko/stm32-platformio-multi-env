#ifndef __USBD_H__
#define __USBD_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
/* Private includes ----------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Operation result types */
typedef enum usb_result_e {
	USB_RESULT_NOTSUPP = 0,
	USB_RESULT_HANDLED = 1,
	USB_RESULT_NEXT_CALLBACK = 2,
} usb_result_t;

/* USB control states */
typedef enum usb_state_e {
    USB_STATE_IDLE,
    USB_STATE_STALLED,
    USB_STATE_DATA_IN,
    USB_STATE_LAST_DATA_IN,
    USB_STATE_DATA_OUT,
    USB_STATE_LAST_DATA_OUT,
    USB_STATE_STATUS_IN,
    USB_STATE_STATUS_OUT,
} usb_state_t;

/* Exported constants --------------------------------------------------------*/
#ifndef USB_MAX_CB_CONTROL
#define USB_MAX_CB_CONTROL 4
#endif

#ifndef USB_MAX_CB_SET_CONFIGURATION
#define USB_MAX_CB_SET_CONFIGURATION 4
#endif

/* Exported macro ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

//TODO public api

#ifdef __cplusplus
}
#endif

#endif // __USBD_H__