
#include "usb_control.h"

#include <stdint.h>

#include "usb_standard.h"

static void usb_control_setup_rd(usb_device_t *dev, usb_request_t *req);
static void usb_control_setup_wr(usb_device_t *dev, usb_request_t *req);


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
	dev->control_state.ctrl_buf = dev->ctrl_buf;//TODO are need copy of pointer???
	dev->control_state.ctrl_len = req->wLength;

	if (usb_control_request_dispatch(dev, req)) {
		if (req->wLength) {
			dev->control_state.needs_zlp = needs_zlp(
                dev->control_state.ctrl_len,
                req->wLength,
                dev->device_descr->bMaxPacketSize0
            );
			/* Go to data out stage if handled. */
			usb_control_send_chunk(dev);
		} else {
			/* Go to status stage if handled. */
			usbd_ep_write_packet(dev, 0, NULL, 0);
			dev->control_state.state = USB_STATE_STATUS_IN;
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
	dev->control_state.ctrl_buf = usbd_dev->ctrl_buf;
	dev->control_state.ctrl_len = 0;

	/* Wait for DATA OUT stage. */
	if (req->wLength > usbd_dev->desc->bMaxPacketSize0) {
		usbd_dev->control_state.state = USB_STATE_DATA_OUT;
	} else {
		usbd_dev->control_state.state = USB_STATE_LAST_DATA_OUT;
	}

	usbd_ep_nak_set(dev, 0, 0);
}

void usb_control_setup(usb_device_t *dev, uint8_t ea)
{
	struct usb_setup_data *req = &dev->control_state.req;
	(void) ea;

	dev->control_state.complete = NULL;

	usbd_ep_nak_set(dev, 0, 1);

	if (req->wLength == 0 || req->bmRequestType & 0x80) {
		usb_control_setup_rd(dev, req);
	} else {
		usb_control_setup_wr(dev, req);
	}
}

void usb_control_out(usb_device_t *dev, uint8_t ea)
{
	(void) ea;

	switch (dev->control_state.state) {
        case USB_STATE_DATA_OUT:
            if (usb_control_recv_chunk(dev) < 0) {
                break;
            }
            if ((dev->control_state.req.wLength - dev->control_state.ctrl_len) <= dev->device_descr->bMaxPacketSize0) {
                dev->control_state.state = USB_STATE_LAST_DATA_OUT;
            }
            break;
        case USB_STATE_LAST_DATA_OUT:
            if (usb_control_recv_chunk(dev) < 0) {
                break;
            }
            /*
            * We have now received the full data payload.
            * Invoke callback to process.
            */
            if (usb_control_request_dispatch(dev, &(dev->control_state.req))) {
                /* Go to status stage on success. */
                usbd_ep_write_packet(dev, 0, NULL, 0);
                dev->control_state.state = USB_STATE_STATUS_IN;
            } else {
                stall_transaction(dev);
            }
            break;
        case USB_STATE_STATUS_OUT:
            usbd_ep_read_packet(dev, 0, NULL, 0);
            dev->control_state.state = USB_STATE_IDLE;
            if (dev->control_state.complete) {
                dev->control_state.complete(dev, &(dev->control_state.req));
            }
            dev->control_state.complete = NULL;
            break;
        default:
            stall_transaction(dev);
	}
}

void usb_control_in(usb_device_t *dev, uint8_t ea)
{
	(void) ea;
	struct usb_setup_data *req = &(dev->control_state.req);

	switch (usbd_dev->control_state.state) {
        case USB_STATE_DATA_IN:
            usb_control_send_chunk(dev);
            break;
        case USB_STATE_LAST_DATA_IN:
            dev->control_state.state = USB_STATE_STATUS_OUT;
            usbd_ep_nak_set(dev, 0, 0);
            break;
        case USB_STATE_STATUS_IN:
            if (dev->control_state.complete) {
                dev->control_state.complete(usbd_dev, &(usbd_dev->control_state.req));
            }

            /* Exception: Handle SET ADDRESS function here... */
            if ((req->bmRequestType == 0) && (req->bRequest == USB_REQUEST_SET_ADDRESS)) {
                usbd_dev->driver->set_address(dev, req->wValue);
            }
            dev->control_state.state = USB_STATE_IDLE;
            break;
        default:
            stall_transaction(dev);
	}
}