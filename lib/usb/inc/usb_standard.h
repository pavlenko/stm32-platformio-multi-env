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

typedef struct usb_device_s usb_device_t;

// Callback types


typedef void (*usb_cb_set_configuration_t)(usb_device_t *dev, uint16_t wValue);
typedef void (*usb_cb_endpoint)(usb_device_t *dev, uint8_t ep);

typedef void (*usb_cb_control_complete_t)(usb_device_t *dev, usb_request_t *req, void *ptr);
typedef usb_result_t (*usb_cb_control_cb_t)(
	usb_device_t *dev,
	usb_request_t *req,
	uint8_t **buf,
	uint16_t *len,
	usb_cb_control_complete_t *complete_cb,
	void *ptr
);

typedef struct usb_cb_control_s {
	usb_cb_control_cb_t cb;
	uint8_t mask;
	uint8_t type;
	void *ptr;
} usb_cb_control_t;

typedef struct usb_control_s {
	usb_state_t state;
	usb_request_t req __attribute__((aligned(4)));
	uint8_t *ctrl_buf;
	uint16_t ctrl_len;
	usb_cb_control_complete_t complete_cb;
	void *complete_arg;//TODO are this needed???
	bool needs_zlp;
} usb_control_t;

typedef struct usb_device_s {
    const usb_device_descriptor_t *device_descr;
	const usb_config_descriptor_t *config_descr;
	//const usb_string_descriptor_t *string_descr; //TODO try descriptors
	const char * const *strings;
	const uint8_t num_strings;

	uint8_t current_address;
    uint8_t current_config;

	uint8_t *ctrl_buf;  /**< Internal buffer used for control transfers */
	uint16_t ctrl_len;

	usb_control_t control;

	//TODO callbacks...
	usb_cb_control_t cb_control[USB_MAX_CB_CONTROL];
	usb_cb_set_configuration_t cb_set_configuration;
} __attribute__((packed)) usb_device_t;

/* Exported functions prototypes ---------------------------------------------*/

// temporary export
usb_result_t _usb_request(usb_device_t *dev, usb_request_t *req, uint8_t **buf, uint16_t *len);

#ifdef __cplusplus
}
#endif

#endif //__USB_STANDARD_H__
