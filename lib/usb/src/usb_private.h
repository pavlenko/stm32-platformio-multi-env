/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USB_PRIVATE_H__
#define __USB_PRIVATE_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

#include "usbd.h"

/* Private includes ----------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))

/* Exported variables --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

/**
 * Handle IN endpoint
 * 
 * @param dev USB device handle structure
 * @param ea  Endpoint address
 * @see usb_setup()
 */
void _usb_control_setup(usb_device_t *dev, uint8_t ea);

/**
 * Handle IN endpoint
 * 
 * @param dev USB device handle structure
 * @param ea  Endpoint address
 * @see usb_setup()
 */
void _usb_control_out(usb_device_t *dev, uint8_t ea);

/**
 * Handle IN endpoint
 * 
 * @param dev USB device handle structure
 * @param ea  Endpoint address
 * @see usb_setup()
 */
void _usb_control_in(usb_device_t *dev, uint8_t ea);

/**
 * Dispatch standard requests
 * 
 * @param dev USB device handle structure
 * @param req USB request structure
 * @param buf user buffer that will receive data
 * @param len # of bytes
 * @return usb_result_t Result code
 * @see usb_control_request_dispatch()
 */
usb_result_t _usb_standard_request(usb_device_t *dev, usb_request_t *req, uint8_t **buf, uint16_t *len);

#ifdef __cplusplus
}
#endif

#endif // __USB_PRIVATE_H__