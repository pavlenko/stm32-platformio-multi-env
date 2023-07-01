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

void usb_control_setup(usb_device_t *dev, uint8_t ea);
void usb_control_out(usb_device_t *dev, uint8_t ea);
void usb_control_in(usb_device_t *dev, uint8_t ea);

/**
 * Set an address
 * 
 * @param dev     USB device handle structure
 * @param address Device address
 */
void usb_set_address(usb_device_t *dev, uint8_t address);

/**
 * Setup an endpoint
 * 
 * @param dev      USB device handle structure
 * @param ep       Full EP address including direction (e.g. 0x01 or 0x81)
 * @param type     Value for bmAttributes (USB_ENDPOINT_*)
 * @param max_size Endpoint max size
 * @param cb       Callback to execute
 */
void usb_ep_setup(usb_device_t *dev, uint8_t ep, uint8_t type, uint16_t max_size, usb_cb_endpoint_t cb);

/**
 * Reset all endpoints
 * 
 * @param dev  USB device handle structure
 */
void usb_ep_reset(usb_device_t *dev);

/**
 * Read a packet from endpoint
 * 
 * @param dev USB device handle structure
 * @param ep  EP address (direction is ignored)
 * @param buf user buffer that will receive data
 * @param len # of bytes
 * @return Actual # of bytes read
 */
uint16_t usb_ep_read_packet(usb_device_t *dev, uint8_t ep, const void *buf, uint16_t len);

/**
 * Write a packet to endpoint
 * 
 * @param dev USB device handle structure
 * @param ep  EP address (direction is ignored)
 * @param buf pointer to user data to write
 * @param len # of bytes
 * @return Actual # of bytes read
 */
uint16_t usb_ep_write_packet(usb_device_t *dev, uint8_t ep, const void *buf, uint16_t len);

/**
 * Set/clr STALL condition on an endpoint
 * 
 * @param dev   USB device handle structure
 * @param ep    Full EP address (with direction bit)
 * @param stall If 0, clear STALL, else set stall.
 */
void usb_ep_stall_set(usb_device_t *dev, uint8_t ep, uint8_t stall);

/**
 * Get STALL status of an endpoint
 * 
 * @param dev USB device handle structure
 * @param ep  Full EP address (with direction bit)
 * @return Non-zero if endpoint is stalled 
 */
uint8_t usb_ep_stall_get(usb_device_t *dev, uint8_t ep);

/**
 * Set an Out endpoint to NAK
 * 
 * @param dev USB device handle structure
 * @param ep  EP address
 * @param nak If non-zero, set NAK
 */
void usb_ep_nak_set(usb_device_t *dev, uint8_t ep, uint8_t nak);

#ifdef __cplusplus
}
#endif

#endif // __USB_PRIVATE_H__