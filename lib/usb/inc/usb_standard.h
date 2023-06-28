/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USB_STANDARD_H__
#define __USB_STANDARD_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* Private includes ----------------------------------------------------------*/
#include "usb_descr.h"
#include "usbd.h"

/* Exported types ------------------------------------------------------------*/

//typedef struct usb_device_s usb_device_t;

// Callback types



// typedef usb_result_t (*usb_cb_control_cb_t)(
//     usb_device_t *dev,
//     usb_request_t *req,
//     uint8_t **buf,
//     uint16_t *len,
//     usb_cb_control_complete_t *complete_cb,
//     void *ptr
// );

// typedef struct usb_cb_control_s {
//     usb_cb_control_cb_t cb;
//     uint8_t mask;
//     uint8_t type;
//     void *ptr;
// } usb_cb_control_t;

/* Exported functions prototypes ---------------------------------------------*/

// temporary export
usb_result_t _usb_request(usb_device_t *dev, usb_request_t *req, uint8_t **buf, uint16_t *len);

#ifdef __cplusplus
}
#endif

#endif //__USB_STANDARD_H__
