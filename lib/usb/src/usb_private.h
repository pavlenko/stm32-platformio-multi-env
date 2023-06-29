/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USB_PRIVATE_H__
#define __USB_PRIVATE_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

/* Private includes ----------------------------------------------------------*/
#include "usb_descr.h"
#include "usbd.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))

/* Exported variables --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

/**
 * Dispatch standard device reuests
 * 
 * @param dev 
 * @param req 
 * @param buf 
 * @param len 
 * @return usb_result_t 
 */
usb_result_t _usb_standard_request(usb_device_t *dev, usb_request_t *req, uint8_t **buf, uint16_t *len);

#ifdef __cplusplus
}
#endif

#endif // __USB_PRIVATE_H__