/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "config.h"
#include "main.h"

/* Private includes ----------------------------------------------------------*/
#include "usb.h"
#include "usb_dfu.h"
#include "descriptors.h"

/* Private typedef -----------------------------------------------------------*/
typedef struct dfu_device_s {
    uint8_t     (*flash)(void *romptr, const void *buf, size_t blksize);
    void        *dptr;
    size_t      remained;
    uint8_t     interface;
    uint8_t     bStatus;
    uint8_t     bState;
} dfu_device_t;

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static uint32_t dfu_buffer[DFU_BUFFER_SIZE];
static usbd_device dfu;
static dfu_device_t dfu_device;

/* Private function prototypes -----------------------------------------------*/
static usbd_respond dfu_config(usbd_device *dev, uint8_t config);
static usbd_respond dfu_control(usbd_device *dev, usbd_ctlreq *req, usbd_rqc_callback *callback);
static void dfu_reset(usbd_device *dev, uint8_t ev, uint8_t ep);
static usbd_respond dfu_dnload(void *buf, size_t blksize);
static usbd_respond dfu_upload(usbd_device *dev, size_t blksize);
static usbd_respond dfu_getstate(uint8_t *buf);
static usbd_respond dfu_getstatus(void *buf);
static usbd_respond dfu_clrstatus();
static usbd_respond dfu_abort();
static usbd_respond dfu_set_idle(void);
static usbd_respond dfu_err_badreq(void);

/* Private user code ---------------------------------------------------------*/

int main(void) {
    dfu_set_idle();
    usbd_init(&dfu, &usbd_hw, DFU_EP0_SIZE, dfu_buffer, sizeof(dfu_buffer));
    usbd_reg_config(&dfu, dfu_config);
    usbd_reg_control(&dfu, dfu_control);
    usbd_reg_descr(&dfu, dfu_get_descriptor);
    while (1) {
        usbd_poll(&dfu);
    }
}

static usbd_respond dfu_config(usbd_device *dev, uint8_t config) {
    switch (config) {
        case 0:
            usbd_reg_event(dev, usbd_evt_reset, 0);
            break;
        case 1:
            usbd_reg_event(dev, usbd_evt_reset, dfu_reset);
            break;
        default:
            return usbd_fail;
    }
    return usbd_ack;
}

static usbd_respond dfu_control (usbd_device *dev, usbd_ctlreq *req, usbd_rqc_callback *callback) {
    (void)callback;
    if ((req->bmRequestType  & (USB_REQ_TYPE | USB_REQ_RECIPIENT)) == (USB_REQ_STANDARD | USB_REQ_INTERFACE)) {
        switch (req->bRequest) {
            case USB_STD_SET_INTERFACE:
                if (req->wIndex != 0) return usbd_fail;
                switch (req->wValue) {
                    case 0:
                        break;
                    default:
                        return usbd_fail;
                }
                dfu_device.interface = req->wValue;
                return dfu_set_idle();
            case USB_STD_GET_INTERFACE:
                req->data[0] = dfu_device.interface;
                return usbd_ack;
            default:
                return usbd_fail;
        }
    }
    if ((req->bmRequestType & (USB_REQ_TYPE | USB_REQ_RECIPIENT)) == (USB_REQ_CLASS | USB_REQ_INTERFACE)) {
        switch (req->bRequest) {
            case USB_DFU_DETACH:
                *callback = (usbd_rqc_callback)dfu_reset;
                return usbd_ack;
            case USB_DFU_DNLOAD:
                if (req->wLength <= DFU_BLOCK_SIZE) {
                    return dfu_dnload(req->data, req->wLength);
                }
                break;
            case USB_DFU_UPLOAD:
                if (req->wLength <= DFU_BLOCK_SIZE) {
                    return dfu_upload(dev, req->wLength);
                }
                break;
            case USB_DFU_GETSTATUS:
                return dfu_getstatus(req->data);
            case USB_DFU_CLRSTATUS:
                return dfu_clrstatus();
            case USB_DFU_GETSTATE:
                return dfu_getstate(req->data);
            case USB_DFU_ABORT:
                return dfu_abort();
            default:
                break;
        }
        return dfu_err_badreq();
    }
    return usbd_fail;
}

static void dfu_reset(usbd_device *dev, uint8_t ev, uint8_t ep) {
    (void)dev;
    (void)ev;
    (void)ep;
    /** TODO : add firmware checkout */
    System_Reset();//TODO add own reset logic
}

static usbd_respond dfu_dnload(void *buf, size_t blksize) {
    switch(dfu_device.bState) {
        case    USB_DFU_STATE_DFU_DNLOADIDLE:
        case    USB_DFU_STATE_DFU_DNLOADSYNC:
        case    USB_DFU_STATE_DFU_IDLE:
            if (blksize == 0) {
                dfu_device.bState = USB_DFU_STATE_DFU_MANIFESTSYNC;
                return usbd_ack;
            }
            if (blksize > dfu_device.remained) {
                dfu_device.bStatus = USB_DFU_STATUS_ERR_ADDRESS;
                dfu_device.bState = USB_DFU_STATE_DFU_ERROR;
                return usbd_ack;
            }
            dfu_device.bStatus = dfu_device.flash(dfu_device.dptr, buf, blksize);

            if (USB_DFU_STATUS_OK == dfu_device.bStatus) {
                dfu_device.dptr += blksize;
                dfu_device.remained -= blksize;
#if (DFU_DNLOAD_NOSYNC == _ENABLE)
                dfu_device.bState = USB_DFU_STATE_DFU_DNLOADIDLE;
#else
                dfu_device.bState = USB_DFU_STATE_DFU_DNLOADSYNC;
#endif
                return usbd_ack;
            } else {
                dfu_device.bState = USB_DFU_STATE_DFU_ERROR;
                return usbd_ack;
            }
        default:
            return dfu_err_badreq();
    }
}

static usbd_respond dfu_upload(usbd_device *dev, size_t blksize) {
    switch (dfu_device.bState) {
        case USB_DFU_STATE_DFU_IDLE:
        case USB_DFU_STATE_DFU_UPLOADIDLE:
            if (dfu_device.remained == 0) {
                dev->status.data_count = 0;
                return dfu_set_idle();
            } else if (dfu_device.remained < DFU_BLOCK_SIZE) {
                blksize = dfu_device.remained;
            }
            dev->status.data_count = blksize;
            dfu_device.remained -= blksize;
            dfu_device.dptr += blksize;
            return usbd_ack;
        default:
            return dfu_err_badreq();
    }
}

static usbd_respond dfu_getstate(uint8_t *buf) {
    *buf = dfu_device.bState;
    return usbd_ack;
}

static usbd_respond dfu_getstatus(void *buf) {
    /* make answer */
    struct usb_dfu_status *stat = buf;
    stat->bStatus = dfu_device.bStatus;
    stat->bState = dfu_device.bState;
    stat->bPollTimeout = (DFU_POLL_TIMEOUT & 0xFF);
    stat->wPollTimeout = (DFU_POLL_TIMEOUT >> 8);
    stat->iString = NO_DESCRIPTOR;

    switch (dfu_device.bState) {
        case USB_DFU_STATE_DFU_IDLE:
        case USB_DFU_STATE_DFU_DNLOADIDLE:
        case USB_DFU_STATE_DFU_UPLOADIDLE:
        case USB_DFU_STATE_DFU_ERROR:
            return usbd_ack;
        case USB_DFU_STATE_DFU_DNLOADSYNC:
            dfu_device.bState = USB_DFU_STATE_DFU_DNLOADIDLE;
            return usbd_ack;
        case USB_DFU_STATE_DFU_MANIFESTSYNC:
            return dfu_set_idle();
        default:
            return dfu_err_badreq();
    }
}

static usbd_respond dfu_clrstatus() {
    if (dfu_device.bState == USB_DFU_STATE_DFU_ERROR)  {
        return dfu_set_idle();
    } else {
        return dfu_err_badreq();
    }
}

static usbd_respond dfu_abort() {
    switch (dfu_device.bState) {
        case USB_DFU_STATE_DFU_IDLE:
        case USB_DFU_STATE_DFU_DNLOADSYNC:
        case USB_DFU_STATE_DFU_DNLOADIDLE:
        case USB_DFU_STATE_DFU_MANIFESTSYNC:
        case USB_DFU_STATE_DFU_UPLOADIDLE:
            return dfu_set_idle();
        default:
            return dfu_err_badreq();
    }
}

static usbd_respond dfu_set_idle(void) {
    dfu_device.bState = USB_DFU_STATE_DFU_IDLE;
    dfu_device.bStatus = USB_DFU_STATUS_OK;
    switch (dfu_device.interface){
        default:
            dfu_device.dptr = (void*)_APP_START;
            dfu_device.remained = FLASH_SIZE - DFU_SIZE;
            dfu_device.flash = program_flash;
            break;
    }
    return usbd_ack;
}

static usbd_respond dfu_err_badreq(void) {
    dfu_device.bState  = USB_DFU_STATE_DFU_ERROR;
    dfu_device.bStatus = USB_DFU_STATUS_ERR_STALLEDPKT;
    return usbd_fail;
}
