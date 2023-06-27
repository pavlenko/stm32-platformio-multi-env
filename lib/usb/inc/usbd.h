#ifndef __USBD_H__
#define __USBD_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdbool.h>
#include <stdint.h>

/* Private includes ----------------------------------------------------------*/
//TODO do not include descriptors if not need
#include "usb_descr.h"

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

typedef struct usb_device_s usb_device_t;

/**
 * Data request callback
 * 
 * @see usb_control_in
 * @see usb_control_out
 * @param dev USB device handle structure
 * @param req USB data request
 * @param ptr Additional data poiter
 */
typedef void (*usb_cb_control_complete_t)(usb_device_t *dev, usb_request_t *req, void *ptr);

/**
 * Control request callback
 * 
 * @see usb_control_request_dispatch
 * @param dev USB device handle structure
 * @param req USB data request
 * @param buf Pointer to data buffer
 * @param len Size of buffer
 * @param ptr Additional data poiter
 */
typedef usb_result_t (*usb_cb_control_t)(
	usb_device_t *dev,
	usb_request_t *req,
	uint8_t **buf,
	uint16_t *len,
	usb_cb_control_complete_t *complete_cb,
	void *ptr
);

/**
 * Set cinfiguration request callback
 * 
 * @see usb_set_configuration
 * @param dev USB device handle structure
 * @param wValue USB SET_CONFIGURATION request wValue field
 */
typedef void (*usb_cb_set_configuration_t)(usb_device_t *dev, uint16_t wValue);

/**
 * Set interface request callback
 * 
 * @see usb_set_interface
 * @param dev USB device handle structure
 * @param wIndex USB SET_INTERFACE request wIndex field
 * @param wValue USB SET_INTERFACE request wValue field
 */
typedef void (*usb_cb_set_interface_t)(usb_device_t *dev, uint16_t wIndex, uint16_t wValue);

/**
 * Endpoint callback
 * 
 * @param dev USB device handle structure
 * @param ep USB endpoint number
 */
typedef void (*usb_cb_endpoint)(usb_device_t *dev, uint8_t ep);

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

/**
 * Register a custom set configuration request callback
 * 
 * @param dev USB device handle structure
 * @param cb Callback to execute
 * @return true(1) on success, else false(0) on failure
 */
bool usb_register_cb_set_configuration(usb_device_t *dev, usb_cb_set_configuration_t cb);

/**
 * Register a custom set interface request callback
 * 
 * @param dev USB device handle structure
 * @param cb Callback to execute
 * @return true(1) on success, else false(0) on failure
 */
bool usb_register_cb_set_interface(usb_device_t *dev, usb_cb_set_interface_t cb);

/**
 * Register a custom control request callback
 * 
 * @param dev USB device handle structure
 * @param type Handled request type
 * @param mask Mask to match type
 * @param cb Callback to execute
 * @return true(1) on success, else false(0) on failure
 */
bool usb_register_cb_control(usb_device_t *dev, uint8_t type, uint8_t mask, usb_cb_control_t cb);

//TODO docs
__attribute__((weak))
void usb_ep_setup(usb_device_t *dev, uint8_t address, uint8_t type, uint16_t max_size, usb_cb_endpoint cb);

__attribute__((weak))
void usb_ep_reset(usb_device_t *dev);

__attribute__((weak))
uint16_t usb_ep_read_packet(usb_device_t *dev, uint8_t address, const void *buf, uint16_t len);

__attribute__((weak))
void usb_ep_write_packet(usb_device_t *dev, uint8_t address, const void *buf, uint16_t len);

__attribute__((weak))
void usb_ep_stall_set(usb_device_t *dev, uint8_t address, uint8_t stall);

__attribute__((weak))
uint8_t usb_ep_stall_get(usb_device_t *dev, uint8_t address);

__attribute__((weak))
void usb_ep_nak_set(usb_device_t *dev, uint8_t address, uint8_t nak);

#ifdef __cplusplus
}
#endif

#endif // __USBD_H__