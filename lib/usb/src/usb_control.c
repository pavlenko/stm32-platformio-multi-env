
#include "usb_control.h"

#include <stdint.h>

#include "usb_standard.h"

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))

static void usb_control_setup_rd(usb_device_t *dev, usb_request_t *req);
static void usb_control_setup_wr(usb_device_t *dev, usb_request_t *req);
static int usb_control_recv_chunk(usb_device_t *dev);
static void usb_control_send_chunk(usb_device_t *dev);
static void usb_stall_transaction(usb_device_t *dev);

//TODO dummy:
bool needs_zlp(uint16_t len, uint16_t wLength, uint8_t ep_size)
{
	return false;
}

static int usb_control_recv_chunk(usb_device_t *dev)
{
	uint16_t packetsize = MIN(
		dev->device_descr->bMaxPacketSize0,
		dev->control.req.wLength - dev->control.ctrl_len
	);
	
	uint16_t size = usb_ep_read_packet(
		dev,
		0,
		dev->control.ctrl_buf + dev->control.ctrl_len,
		packetsize
	);

	if (size != packetsize) {
		usb_stall_transaction(dev);
		return -1;
	}

	dev->control.ctrl_len += size;

	return packetsize;
}

static void usb_control_send_chunk(usb_device_t *dev)
{
	if (dev->device_descr->bMaxPacketSize0 < dev->control.ctrl_len) {
		/* Data stage, normal transmission */
		usb_ep_write_packet(
			dev,
			0,
			dev->control.ctrl_buf,
			dev->device_descr->bMaxPacketSize0
		);

		dev->control.state = USB_STATE_DATA_IN;
		dev->control.ctrl_buf += dev->device_descr->bMaxPacketSize0;
		dev->control.ctrl_len -= dev->device_descr->bMaxPacketSize0;
	} else {
		/* Data stage, end of transmission */
		usb_ep_write_packet(
			dev,
			0,
			dev->control.ctrl_buf,
			dev->control.ctrl_len
		);

		dev->control.state = dev->control.needs_zlp ? USB_STATE_DATA_IN : USB_STATE_LAST_DATA_IN;
		dev->control.needs_zlp = false;
		dev->control.ctrl_len = 0;
		dev->control.ctrl_buf = NULL;
	}
}

static void usb_stall_transaction(usb_device_t *dev)
{
	usb_ep_stall_set(dev, 0, 1);
	dev->control.state = USB_STATE_IDLE;
}

usb_result_t usb_control_request_dispatch(usb_device_t *dev, usb_request_t *req)
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
				&(dev->control.ctrl_buf),
				&(dev->control.ctrl_len),
				&(dev->control.complete_cb),
				dev->cb_control[i].ptr
			);
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
			usb_ep_write_packet(dev, 0, NULL, 0);
			dev->control.state = USB_STATE_STATUS_IN;
		}
	} else {
		usb_stall_transaction(dev);/* Stall endpoint on failure. */
	}
}

static void usb_control_setup_wr(usb_device_t *dev, usb_request_t *req)
{
	if (req->wLength > dev->ctrl_len) {
		usb_stall_transaction(dev);
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

	usb_ep_nak_set(dev, 0, 0);
}

void usb_control_setup(usb_device_t *dev, uint8_t ea)
{
	(void) ea;

	dev->control.complete_cb = NULL;

	usb_ep_nak_set(dev, 0, 1);

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
                usb_ep_write_packet(dev, 0, NULL, 0);
                dev->control.state = USB_STATE_STATUS_IN;
            } else {
                usb_stall_transaction(dev);
            }
            break;
        case USB_STATE_STATUS_OUT:
            usb_ep_read_packet(dev, 0, NULL, 0);
            dev->control.state = USB_STATE_IDLE;
            if (dev->control.complete_cb) {
                dev->control.complete_cb(dev, &(dev->control.req), dev->control.complete_arg);
            }
            dev->control.complete_cb = NULL;
            break;
        default:
            usb_stall_transaction(dev);
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
            usb_ep_nak_set(dev, 0, 0);
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
            usb_stall_transaction(dev);
	}
}