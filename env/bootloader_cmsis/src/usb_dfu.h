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

#define DFU_CMD_SET_ADDRESS 0x21
#define DFU_CMD_ERASE       0x41

typedef enum dfu_request_s {
	DFU_REQUEST_DETACH,
	DFU_REQUEST_DNLOAD,
	DFU_REQUEST_UPLOAD,
	DFU_REQUEST_GET_STATUS,
	DFU_REQUEST_CLR_STATUS,
	DFU_REQUEST_GET_STATE,
	DFU_REQUEST_ABORT,
} dfu_request_t;

typedef enum dfu_status_e {
	DFU_STATUS_OK,
	DFU_STATUS_ERR_TARGET,
	DFU_STATUS_ERR_FILE,
	DFU_STATUS_ERR_WRITE,
	DFU_STATUS_ERR_ERASE,
	DFU_STATUS_ERR_CHECK_ERASED,
	DFU_STATUS_ERR_PROG,
	DFU_STATUS_ERR_VERIFY,
	DFU_STATUS_ERR_ADDRESS,
	DFU_STATUS_ERR_NOTDONE,
	DFU_STATUS_ERR_FIRMWARE,
	DFU_STATUS_ERR_VENDOR,
	DFU_STATUS_ERR_USBR,
	DFU_STATUS_ERR_POR,
	DFU_STATUS_ERR_UNKNOWN,
	DFU_STATUS_ERR_STALLEDPKT,
} dfu_status_t;

typedef enum dfu_state_e {
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
} dfu_state_t;

typedef struct {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bmAttributes;
    uint16_t wDetachTimeout;
    uint16_t wTransferSize;
    uint16_t bcdDFUVersion;
} __attribute__((packed)) dfu_functional_descriptor_t;

typedef struct {
	dfu_state_t state;
	uint8_t *buf;
    uint16_t len;
    uint32_t address;
    uint16_t block_num;
} dfu_t;

usb_result_t dfu_control_request(usb_device_t *dev, usb_request_t *req, uint8_t **buf, uint16_t *len);

__attribute__((weak))
void dfu_system_reset();

__attribute__((weak))
void dfu_memory_unlock();

__attribute__((weak))
void dfu_memory_lock();

__attribute__((weak))
void dfu_memory_erase_page(uint32_t address);

__attribute__((weak))
void dfu_memory_write_uint16(uint32_t address, uint16_t data);

#ifdef __cplusplus
}
#endif

#endif //__USB_DFU_H
