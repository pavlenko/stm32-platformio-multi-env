#include "usb_dfu.h"

#include <string.h>

static dfu_state_t dfu_state = DFU_STATE_DFU_IDLE;
static struct {
    uint8_t *buf;
    uint16_t len;
    uint32_t addr;
    uint16_t blocknum;
} prog;

uint8_t dfu_get_status(usb_device_t *dev, uint32_t *bwPollTimeout)
{
    switch (dfu_state) {
        case DFU_STATE_DFU_DNLOAD_SYNC:
            dfu_state = DFU_STATE_DFU_DNBUSY;
            *bwPollTimeout = 100;
            return DFU_STATUS_OK;
        case DFU_STATE_DFU_MANIFEST_SYNC:/* Device will reset when read is complete. */
            dfu_state = DFU_STATE_DFU_MANIFEST;
            return DFU_STATUS_OK;
        default:
            return DFU_STATUS_OK;
    }
}

usb_result_t dfu_control_request(usb_device_t *dev, usb_request_t *req, uint8_t **buf, uint16_t *len)
{
    if ((req->bmRequestType & 0x7F) != DFU_DESCRIPTOR_TYPE_FUNCTIONAL) {
        return USB_RESULT_NOTSUPP;
    }
    uint32_t bwPollTimeout = 0;
    switch (req->bRequest) {
        case DFU_REQUEST_DNLOAD:
            if (len == NULL || *len == 0) {
                dfu_state = DFU_STATE_DFU_MANIFEST_SYNC;
            } else {/* Copy download data for use on GET_STATUS. */
                prog.blocknum = req->wValue;
                prog.len = *len;
                memcpy(prog.buf, *buf, *len);
                dfu_state = DFU_STATE_DFU_DNLOAD_SYNC;
            }
            return USB_RESULT_HANDLED;/* Clear error and return to dfuIDLE. */
        case DFU_REQUEST_CLR_STATUS:
            if (dfu_state == DFU_STATE_DFU_ERROR) {
                dfu_state = DFU_STATE_DFU_IDLE;
            }
            return USB_RESULT_HANDLED;
        case DFU_REQUEST_ABORT:/* Abort returns to dfuIDLE state. */
            dfu_state = DFU_STATE_DFU_IDLE;
            return USB_RESULT_HANDLED;
        case DFU_REQUEST_DETACH:
            dfu_state = DFU_STATE_DFU_MANIFEST_SYNC;
            //TODO *complete = usbdfu_getstatus_complete;
            return USB_RESULT_HANDLED;
        case DFU_REQUEST_UPLOAD:/* Upload not supported for now. */
            return USB_RESULT_NOTSUPP;
        case DFU_REQUEST_GET_STATUS:
            (*buf)[0] = dfu_get_status(dev, &bwPollTimeout);
            (*buf)[1] = bwPollTimeout & 0xFF;
            (*buf)[2] = (bwPollTimeout >> 8) & 0xFF;
            (*buf)[3] = (bwPollTimeout >> 16) & 0xFF;
            (*buf)[4] = dfu_state;
            (*buf)[5] = 0; /* iString not used here */
            *len = 6;
            //TODO *complete = usbdfu_getstatus_complete;
            return USB_RESULT_HANDLED;
        case DFU_REQUEST_GET_STATE:
            *buf[0] = dfu_state;
            *len = 1;
            return USB_RESULT_HANDLED;
    }

    return USB_RESULT_NOTSUPP;
}