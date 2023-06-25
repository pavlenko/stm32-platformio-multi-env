#ifndef __USB_DFU_H
#define __USB_DFU_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"
#include "usb_standard.h"

#define USB_CLASS_DFU 0xFE

#define DFU_DESCRIPTOR_TYPE_FUNCTIONAL 0x21

#define DFU_ATTR_CAN_DOWNLOAD      0x01
#define DFU_ATTR_CAN_UPLOAD        0x02
#define DFU_ATTR_MANIFEST_TOLERANT 0x04
#define DFU_ATTR_WILL_DETACH       0x08

enum {
	DFU_REQUEST_DETACH,
	DFU_REQUEST_DNLOAD,
	DFU_REQUEST_UPLOAD,
	DFU_REQUEST_GET_STATUS,
	DFU_REQUEST_CLR_STATUS,
	DFU_REQUEST_GET_STATE,
	DFU_REQUEST_ABORT,
};

enum dfu_state {
	DFU_STATE_APP_IDLE,
	DFU_STATE_APP_DETACH,
	DFU_STATE_DFU_IDLE,
	DFU_STATE_DFU_DNLOAD_SYNC,
	DFU_STATE_DFU_DNBUSY,
	DFU_STATE_DFU_DNLOAD_IDLE,
	DFU_STATE_DFU_MANIFEST_SYNC,
	DFU_STATE_DFU_MANIFEST,
	DFU_STATE_DFU_MANIFEST_WAIT_RESET,
	DFU_STATE_DFU_UPLOAD_IDLE,
	DFU_STATE_DFU_ERROR,
};

typedef struct {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bmAttributes;
    uint16_t wDetachTimeout;
    uint16_t wTransferSize;
    uint16_t bcdDFUVersion;
} __attribute__((packed)) dfu_functional_descriptor_t;

usb_result_t dfu_control_request(usb_device_t *dev, usb_request_t *req, uint8_t **buf, uint16_t *len);

#ifdef __cplusplus
}
#endif

#endif //__USB_DFU_H
