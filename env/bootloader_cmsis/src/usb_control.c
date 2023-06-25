
#include "usb_control.h"

#include <stdint.h>

#include "usb_standard.h"

static void usb_control_setup_rd(usb_device_t *dev, usb_request_t *req);
static void usb_control_setup_wr(usb_device_t *dev, usb_request_t *req);

//TODO dummy:
bool needs_zlp(uint16_t len, uint16_t wLength, uint8_t ep_size)
{
	return false;
}

int usb_control_recv_chunk(usb_device_t *dev)
{
	return -1;
}

void usb_control_send_chunk(usb_device_t *dev)
{}

void usbd_ep_setup(usb_device_t *dev, uint8_t addr, uint8_t type, uint16_t max_size, usb_cb_endpoint cb)
{}

void usb_ep_read_packet(usb_device_t *dev, uint8_t addr, const void *buf, uint16_t len)
{}

void usb_ep_write_packet(usb_device_t *dev, uint8_t addr, const void *buf, uint16_t len)
{}

void usbd_ep_stall_set(usb_device_t *dev, uint8_t addr, uint8_t stall)
{}

uint8_t usbd_ep_stall_get(usb_device_t *dev, uint8_t addr)
{
	return 0;
}

void usbd_ep_nak_set(usb_device_t *dev, uint8_t addr, uint8_t nak)
{}

void stall_transaction(usb_device_t *dev)
{}
//END dummy:


static usb_result_t usb_control_request_dispatch(usb_device_t *dev, usb_request_t *req)
{
	uint8_t i;
	usb_result_t result;

	for (i = 0; i < USB_MAX_CB_CONTROL; i++) {
		if (dev->cb_control[i].cb == NULL) {
			break;
		}
		if ((req->bmRequestType & dev->cb_control[i].mask) == dev->cb_control[i].type) {
			result = dev->cb_control[i].cb(
				dev,
				 req,
				&(dev->ctrl_buf),
				&(dev->ctrl_len)
			);//TODO pass onComplete callback???
			if (result == USB_RESULT_HANDLED || result == USB_RESULT_NOTSUPP) {
				return result;
			}
		}
	}

	return _usb_request(dev, req, &(dev->ctrl_buf), &(dev->ctrl_len));
}

static void usb_control_setup_rd(usb_device_t *dev, usb_request_t *req)
{
	dev->control.ctrl_buf = dev->ctrl_buf;
	dev->control.ctrl_len = req->wLength;

	if (usb_control_request_dispatch(dev, req)) {
		if (req->wLength) {
			dev->control.needs_zlp = needs_zlp(
                dev->control.ctrl_len,
                req->wLength,
                dev->device_descr->bMaxPacketSize0
            );
			/* Go to data out stage if handled. */
			usb_control_send_chunk(dev);
		} else {
			/* Go to status stage if handled. */
			usbd_ep_write_packet(dev, 0, NULL, 0);
			dev->control.state = USB_STATE_STATUS_IN;
		}
	} else {
		/* Stall endpoint on failure. */
		stall_transaction(dev);
	}
}

static void usb_control_setup_wr(usb_device_t *dev, usb_request_t *req)
{
	if (req->wLength > dev->ctrl_len) {
		stall_transaction(dev);
		return;
	}

	/* Buffer into which to write received data. */
	dev->control.ctrl_buf = dev->ctrl_buf;
	dev->control.ctrl_len = 0;

	/* Wait for DATA OUT stage. */
	if (req->wLength > dev->device_descr->bMaxPacketSize0) {
		dev->control.state = USB_STATE_DATA_OUT;
	} else {
		dev->control.state = USB_STATE_LAST_DATA_OUT;
	}

	usbd_ep_nak_set(dev, 0, 0);
}

void usb_control_setup(usb_device_t *dev, uint8_t ea)
{
	(void) ea;

	dev->control.complete_cb = NULL;

	usbd_ep_nak_set(dev, 0, 1);

	if (dev->control.req.wLength == 0 || dev->control.req.bmRequestType & 0x80) {
		usb_control_setup_rd(dev, &dev->control.req);
	} else {
		usb_control_setup_wr(dev, &dev->control.req);
	}
}

void usb_control_out(usb_device_t *dev, uint8_t ea)
{
	(void) ea;

	switch (dev->control.state) {
        case USB_STATE_DATA_OUT:
            if (usb_control_recv_chunk(dev) < 0) {
                break;
            }
            if ((dev->control.req.wLength - dev->control.ctrl_len) <= dev->device_descr->bMaxPacketSize0) {
                dev->control.state = USB_STATE_LAST_DATA_OUT;
            }
            break;
        case USB_STATE_LAST_DATA_OUT:
            if (usb_control_recv_chunk(dev) < 0) {
                break;
            }
            /* We have now received the full data payload. Invoke callback to process. */
            if (usb_control_request_dispatch(dev, &(dev->control.req))) {
                /* Go to status stage on success. */
                usbd_ep_write_packet(dev, 0, NULL, 0);
                dev->control.state = USB_STATE_STATUS_IN;
            } else {
                stall_transaction(dev);
            }
            break;
        case USB_STATE_STATUS_OUT:
            usbd_ep_read_packet(dev, 0, NULL, 0);
            dev->control.state = USB_STATE_IDLE;
            if (dev->control.complete_cb) {
                dev->control.complete_cb(dev, &(dev->control.req), dev->control.complete_arg);
            }
            dev->control.complete_cb = NULL;
            break;
        default:
            stall_transaction(dev);
	}
}

void usb_control_in(usb_device_t *dev, uint8_t ea)
{
	(void) ea;
	usb_request_t *req = &(dev->control.req);

	switch (dev->control.state) {
        case USB_STATE_DATA_IN:
            usb_control_send_chunk(dev);
            break;
        case USB_STATE_LAST_DATA_IN:
            dev->control.state = USB_STATE_STATUS_OUT;
            usbd_ep_nak_set(dev, 0, 0);
            break;
        case USB_STATE_STATUS_IN:
            if (dev->control.complete_cb) {
                dev->control.complete_cb(dev, req, dev->control.complete_arg);
            }

            /* Exception: Handle SET ADDRESS function here... */
            if ((req->bmRequestType == 0) && (req->bRequest == USB_REQUEST_SET_ADDRESS)) {
                //TODO dev->driver->set_address(dev, req->wValue);
            }
            dev->control.state = USB_STATE_IDLE;
            break;
        default:
            stall_transaction(dev);
	}
}