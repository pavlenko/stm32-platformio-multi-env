#ifndef __USB_CONTROL_H
#define __USB_CONTROL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "usb_standard.h"

// Temporary export
usb_result_t usb_control_request_dispatch(usb_device_t *dev, usb_request_t *req);

// Weak functions for hardware dependent
__attribute__((weak))
void usb_ep_setup(usb_device_t *dev, uint8_t address, uint8_t type, uint16_t max_size, usb_cb_endpoint cb);

__attribute__((weak))
void usb_ep_reset(usb_device_t *dev);

__attribute__((weak))
uint16_t usb_ep_read_packet(usb_device_t *dev, uint8_t address, const void *buf, uint16_t len);

__attribute__((weak))
uint16_t usb_ep_write_packet(usb_device_t *dev, uint8_t address, const void *buf, uint16_t len);

__attribute__((weak))
void usb_ep_stall_set(usb_device_t *dev, uint8_t address, uint8_t stall);

__attribute__((weak))
uint8_t usb_ep_stall_get(usb_device_t *dev, uint8_t address);

__attribute__((weak))
void usb_ep_nak_set(usb_device_t *dev, uint8_t address, uint8_t nak);

#ifdef __cplusplus
}
#endif

#endif //__USB_CONTROL_H