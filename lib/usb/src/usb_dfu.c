#include "usb_dfu.h"

#include <string.h>

#include "usb_standard.h"

uint8_t dfu_get_status(dfu_t *dfu, uint32_t *bwPollTimeout)
{
    switch (dfu->state) {
        case DFU_STATE_DFU_DNLOAD_SYNC:
            dfu->state = DFU_STATE_DFU_DNBUSY;
            *bwPollTimeout = 100;
            return DFU_STATUS_OK;
        case DFU_STATE_DFU_MANIFEST_SYNC:/* Device will reset when read is complete. */
            dfu->state = DFU_STATE_DFU_MANIFEST;
            return DFU_STATUS_OK;
        default:
            return DFU_STATUS_OK;
    }
}

static void dfu_cb_control_complete(usb_device_t *dev, usb_request_t *req, void *ptr)
{
    (void) dev;
    (void) req;

    uint16_t i;
    dfu_t *dfu = (dfu_t *) ptr;
    switch (dfu->state) {
        case DFU_STATE_DFU_DNBUSY:
            dfu_memory_unlock();
            if (dfu->block_num == 0) {
                switch (dfu->buf[0]) {
                    case DFU_CMD_ERASE:
                        dfu_memory_erase_page(*(uint32_t *)(dfu->buf + 1));
                        break;
                    case DFU_CMD_SET_ADDRESS:
                        dfu->address = *(uint32_t *)(dfu->buf + 1);
                        break;
                }
            } else {
                uint32_t base_address = dfu->address + ((dfu->block_num - 2) * dfu->descr->wTransferSize);
                for (i = 0; i < dfu->len; i += 2) {
                    dfu_memory_write_uint16(base_address + i, *(uint16_t *)(dfu->buf + i));
                }
            }
            dfu_memory_lock();

            /* Jump straight to dfuDNLOAD-IDLE, skipping dfuDNLOAD-SYNC. */
            dfu->state = DFU_STATE_DFU_DNLOAD_IDLE;
            return;
        case DFU_STATE_DFU_MANIFEST:
            dfu_system_reset();/* USB device must detach, we just reset... */
            return;
        default:
            return;
    }
}

//TODO avoid use ptr, resolve it in other way
usb_result_t dfu_cb_control(
    usb_device_t *dev,
    usb_request_t *req,
    uint8_t **buf,
    uint16_t *len,
    usb_cb_control_complete_t *cb,
    void *ptr
) {
    uint32_t bwPollTimeout = 0;
    dfu_t *dfu = (dfu_t *) ptr;
    if ((req->bmRequestType & 0x7F) != DFU_DESCRIPTOR_TYPE_FUNCTIONAL) {
        return USB_RESULT_NOTSUPP;
    }
    switch (req->bRequest) {
        case DFU_REQUEST_DNLOAD:
            if (len == NULL || *len == 0) {
                dfu->state = DFU_STATE_DFU_MANIFEST_SYNC;
            } else {/* Copy download data for use on GET_STATUS. */
                dfu->block_num = req->wValue;
                dfu->len = *len;
                memcpy(dfu->buf, *buf, *len);
                dfu->state = DFU_STATE_DFU_DNLOAD_SYNC;
            }
            return USB_RESULT_HANDLED;/* Clear error and return to dfuIDLE. */
        case DFU_REQUEST_CLR_STATUS:
            if (dfu->state == DFU_STATE_DFU_ERROR) {
                dfu->state = DFU_STATE_DFU_IDLE;
            }
            return USB_RESULT_HANDLED;
        case DFU_REQUEST_ABORT:/* Abort returns to dfuIDLE state. */
            dfu->state = DFU_STATE_DFU_IDLE;
            return USB_RESULT_HANDLED;
        case DFU_REQUEST_DETACH:
            dfu->state = DFU_STATE_DFU_MANIFEST_SYNC;
            *cb = dfu_cb_control_complete;
            return USB_RESULT_HANDLED;
        case DFU_REQUEST_UPLOAD:/* Upload not supported for now. */
            return USB_RESULT_NOTSUPP;
        case DFU_REQUEST_GET_STATUS:
            (*buf)[0] = dfu_get_status(dfu, &bwPollTimeout);
            (*buf)[1] = bwPollTimeout & 0xFF;
            (*buf)[2] = (bwPollTimeout >> 8) & 0xFF;
            (*buf)[3] = (bwPollTimeout >> 16) & 0xFF;
            (*buf)[4] = dfu->state;
            (*buf)[5] = 0; /* iString not used here */
            *len = 6;
            *cb = dfu_cb_control_complete;
            return USB_RESULT_HANDLED;
        case DFU_REQUEST_GET_STATE:
            *buf[0] = dfu->state;
            *len = 1;
            return USB_RESULT_HANDLED;
    }

    return USB_RESULT_NOTSUPP;
}
