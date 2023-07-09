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
/* Exported functions prototypes ---------------------------------------------*/

// temporary export
usb_result_t _usb_standard_request(usb_device_t *dev, usb_request_t *req, uint8_t **buf, uint16_t *len);

#ifdef __cplusplus
}
#endif

#endif //__USB_STANDARD_H__
