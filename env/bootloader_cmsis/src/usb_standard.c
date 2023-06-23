#include "usb_standard.h"

/* Private imports -----------------------------------------------------------*/
#include <string.h>

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))

/* Private function prototypes -----------------------------------------------*/
static usb_result_t usb_get_status_device(usb_device_t *dev, usb_request_t *req, uint8_t **buf, uint16_t *len);
static usb_result_t usb_get_status_interface(usb_device_t *dev, usb_request_t *req, uint8_t **buf, uint16_t *len);
static usb_result_t usb_get_status_endpoint(usb_device_t *dev, usb_request_t *req, uint8_t **buf, uint16_t *len);
// No need for: usb_clr_feature()
// No need for: usb_set_feature()
static usb_result_t usb_set_address(usb_device_t *dev, usb_request_t *req, uint8_t **buf, uint16_t *len);
static usb_result_t usb_get_descriptor(usb_device_t *dev, usb_request_t *req, uint8_t **buf, uint16_t *len);
// No need for: usb_set_descriptor()
static usb_result_t usb_get_configuration(usb_device_t *dev, usb_request_t *req, uint8_t **buf, uint16_t *len);
static usb_result_t usb_set_configuration(usb_device_t *dev, usb_request_t *req, uint8_t **buf, uint16_t *len);
static usb_result_t usb_get_interface(usb_device_t *dev, usb_request_t *req, uint8_t **buf, uint16_t *len);
static usb_result_t usb_set_interface(usb_device_t *dev, usb_request_t *req, uint8_t **buf, uint16_t *len);

static usb_result_t _usb_endpoint_stall(usb_device_t *dev, usb_request_t *req, uint8_t **buf, uint16_t *len);
static usb_result_t _usb_endpoint_unstall(usb_device_t *dev, usb_request_t *req, uint8_t **buf, uint16_t *len);
static uint8_t _usb_endpoint_stall_get(usb_device_t *dev, uint8_t addr);

static usb_result_t _usb_request_device(usb_device_t *dev, usb_request_t *req, uint8_t **buf, uint16_t *len);
static usb_result_t _usb_request_interface(usb_device_t *dev, usb_request_t *req, uint8_t **buf, uint16_t *len);
static usb_result_t _usb_request_endpoint(usb_device_t *dev, usb_request_t *req, uint8_t **buf, uint16_t *len);


/* Private function code -----------------------------------------------------*/
static usb_result_t usb_get_status_device(usb_device_t *dev, usb_request_t *req, uint8_t **buf, uint16_t *len)
{
    (void) dev;
	(void) req;

	//TODO check: bit 0: self powered, bit 1: remote wakeup
	if (*len > 2) {
		*len = 2;
	}

	(*buf)[0] = 0;
	(*buf)[1] = 0;

	return USB_RESULT_HANDLED;
}

static usb_result_t usb_get_status_interface(usb_device_t *dev, usb_request_t *req, uint8_t **buf, uint16_t *len)
{
	(void) dev;
	(void) req;
	/* not defined */
	if (*len > 2) {
		*len = 2;
	}
	(*buf)[0] = 0;
	(*buf)[1] = 0;

	return USB_RESULT_HANDLED;
}

static usb_result_t usb_get_status_endpoint(usb_device_t *dev, usb_request_t *req, uint8_t **buf, uint16_t *len)
{
	(void) req;

	if (*len > 2) {
		*len = 2;
	}
	(*buf)[0] = _usb_endpoint_stall_get(dev, req->wIndex) ? 1 : 0;//TODO check
	(*buf)[1] = 0;

	return USB_RESULT_HANDLED;
}

static usb_result_t usb_set_address(usb_device_t *dev, usb_request_t *req, uint8_t **buf, uint16_t *len)
{
    (void) req;
	(void) buf;
	(void) len;

	/* The actual address is only latched at the STATUS IN stage. */
	if ((req->bmRequestType != 0) || (req->wValue >= 128)) {
		return USB_RESULT_NOTSUPP;
	}

	dev->current_address = req->wValue;

	/*
	 * Special workaround for STM32F10[57] that require the address
	 * to be set here. This is undocumented!
	 */
	// if (dev->driver->set_address_before_status) {
	// 	dev->driver->set_address(dev, req->wValue);//TODO check
	// }

	return USB_RESULT_HANDLED;
}

static usb_result_t usb_get_descriptor(usb_device_t *dev, usb_request_t *req, uint8_t **buf, uint16_t *len)
{
	int i, array_idx, descr_idx;
	usb_string_descriptor_t *sd;

	descr_idx = (int)(req->wValue & 0xFF);

	switch (req->wValue >> 8) {
		case USB_DESCRIPTOR_TYPE_DEVICE:
			*buf = (uint8_t *) dev->device_descr;
			*len = MIN(*len, dev->device_descr->bLength);
			return USB_RESULT_HANDLED;
		case USB_DESCRIPTOR_TYPE_CONFIG:
			//*buf = usbd_dev->ctrl_buf;//TODO
			//*len = build_config_descriptor(usbd_dev, descr_idx, *buf, *len);//TODO
			return USB_RESULT_HANDLED;
		case USB_DESCRIPTOR_TYPE_STRING:
			sd = (usb_string_descriptor_t *) dev->ctrl_buf;

			if (descr_idx == 0) {
				/* Send sane Language ID descriptor... */
				sd->wData[0] = USB_LANGID_ENGLISH_US;
				sd->bLength = sizeof(sd->bLength) + sizeof(sd->bDescriptorType) + sizeof(sd->wData[0]);

				*len = MIN(*len, sd->bLength);
			} else {
				array_idx = descr_idx - 1;

				if (!dev->strings) {					
					return USB_RESULT_NOTSUPP;// Device doesn't support strings.
				}

				
				if (array_idx >= dev->num_strings) {
					return USB_RESULT_NOTSUPP;// String index is not in range.
				}

				/* Strings with Language ID differnet from
				* USB_LANGID_ENGLISH_US are not supported */
				if (req->wIndex != USB_LANGID_ENGLISH_US) {
					return USB_RESULT_NOTSUPP;
				}

				/* This string is returned as UTF16, hence the
				* multiplication
				*/
				sd->bLength = (strlen(dev->strings[array_idx]) * 2) + sizeof(sd->bLength) + sizeof(sd->bDescriptorType);

				*len = MIN(*len, sd->bLength);

				for (i = 0; i < (*len / 2) - 1; i++) {
					sd->wData[i] = dev->strings[array_idx][i];
				}
			}

			sd->bDescriptorType = USB_DESCRIPTOR_TYPE_STRING;
			*buf = (uint8_t *) sd;

			return USB_RESULT_HANDLED;
	}

	return USB_RESULT_NOTSUPP;
}

static usb_result_t usb_get_configuration(usb_device_t *dev, usb_request_t *req, uint8_t **buf, uint16_t *len)
{
    (void) req;

	if (*len > 1) {
		*len = 1;
	}
	
    if (dev->current_config > 0) {
		const usb_config_descriptor_t *config = &dev->config_descr[dev->current_config - 1];
		(*buf)[0] = config->bConfigurationValue;
	} else {
		(*buf)[0] = 0;
	}

    return USB_RESULT_HANDLED;
}

static usb_result_t usb_set_configuration(usb_device_t *dev, usb_request_t *req, uint8_t **buf, uint16_t *len)
{
    uint8_t i;
	int8_t found_index = -1;
	const usb_config_descriptor_t *config_descr;

	(void) req;
	(void) buf;
	(void) len;

	if (req->wValue > 0) {
		for (i = 0; i < dev->device_descr->bNumConfigurations; i++) {
			if (req->wValue == dev->config_descr[i].bConfigurationValue) {
				found_index = i;
				break;
			}
		}
		if (found_index < 0) {
			return USB_RESULT_NOTSUPP;
		}
	}

	dev->current_config = found_index + 1;

	if (dev->current_config > 0) {
		config_descr = &dev->config_descr[dev->current_config - 1];

		/* reset all alternate settings configuration */
		for (i = 0; i < config_descr->bNumInterfaces; i++) {
			if (config_descr->interface[i].cur_altsetting) {
				*config_descr->interface[i].cur_altsetting = 0;
			}
		}
	}


    _usb_reset_endpoints(dev);

    //TODO if isset user set config callback 0:
    //TODO reset all user controll callbacks
    //TODO call all user set config callbacks
	return USB_RESULT_HANDLED;
}

static usb_result_t usb_get_interface(usb_device_t *dev, usb_request_t *req, uint8_t **buf, uint16_t *len)
{
	uint8_t *cur_altsetting;
	const usb_config_descriptor_t *config_descr = &dev->config_descr[dev->current_config - 1];

	if (req->wIndex >= config_descr->bNumInterfaces) {
		return USB_RESULT_NOTSUPP;
	}

	*len = 1;
	cur_altsetting = config_descr->interface[req->wIndex].cur_altsetting;//TODO optimize
	(*buf)[0] = (cur_altsetting) ? *cur_altsetting : 0;

	return USB_RESULT_HANDLED;
}

static usb_result_t usb_set_interface(usb_device_t *dev, usb_request_t *req, uint8_t **buf, uint16_t *len)
{
	const usb_config_descriptor_t *config_descr = &dev->config_descr[dev->current_config - 1];
	const struct usb_interface *iface;

	(void)buf;

	if (req->wIndex >= config_descr->bNumInterfaces) {
		return USB_RESULT_NOTSUPP;
	}

	iface = &config_descr->interface[req->wIndex];

	if (req->wValue >= iface->num_altsetting) {
		return USB_RESULT_NOTSUPP;
	}

	if (iface->cur_altsetting) {
		*iface->cur_altsetting = req->wValue;
	} else if (req->wValue > 0) {
		return USB_RESULT_NOTSUPP;
	}

	// if (dev->user_callback_set_altsetting) {//TODO callback???
	// 	dev->user_callback_set_altsetting(dev, req->wIndex, req->wValue);
	// }

	*len = 0;

	return USB_RESULT_HANDLED;
}

static usb_result_t _usb_endpoint_stall(usb_device_t *dev, usb_request_t *req, uint8_t **buf, uint16_t *len)
{
	(void) dev;
	(void) req;
	(void) buf;
	(void) len;
	return USB_RESULT_HANDLED;
}

static usb_result_t _usb_endpoint_unstall(usb_device_t *dev, usb_request_t *req, uint8_t **buf, uint16_t *len)
{
	(void) dev;
	(void) req;
	(void) buf;
	(void) len;
	return USB_RESULT_HANDLED;
}

static uint8_t _usb_endpoint_stall_get(usb_device_t *dev, uint8_t addr)
{
	(void) dev;
	(void) addr;
	return 0;
}


static usb_result_t _usb_request_device(usb_device_t *dev, usb_request_t *req, uint8_t **buf, uint16_t *len)
{
    switch (req->bRequest) {
        case USB_REQUEST_GET_STATUS:
            /*
            * GET_STATUS always responds with zero reply.
            * The application may override this behaviour.
            */
            return usb_get_status_device(dev, req, buf, len);
        case USB_REQUEST_CLR_FEATURE:
        case USB_REQUEST_SET_FEATURE:
            // Not implemented, just known
            break;
        case USB_REQUEST_SET_ADDRESS:
            /*
            * SET ADDRESS is an exception.
            * It is only processed at STATUS stage.
            */
            return usb_set_address(dev, req, buf, len);
        case USB_REQUEST_GET_DESCRIPTOR:
            return usb_get_descriptor(dev, req, buf, len);
        case USB_REQUEST_SET_DESCRIPTOR:
            /* SET_DESCRIPTOR is optional and not implemented. */
            break;
        case USB_REQUEST_SET_CONFIGURATION:
            return usb_set_configuration(dev, req, buf, len);
        case USB_REQUEST_GET_CONFIGURATION:
            return usb_get_configuration(dev, req, buf, len);
            break;
	}

	return USB_RESULT_NOTSUPP;
}

static usb_result_t _usb_request_interface(usb_device_t *dev, usb_request_t *req, uint8_t **buf, uint16_t *len)
{
    switch (req->bRequest) {
        case USB_REQUEST_CLR_FEATURE:
        case USB_REQUEST_SET_FEATURE:
            // Not implemented, just known
            break;
        case USB_REQUEST_GET_INTERFACE:
            return usb_get_interface(dev, req, buf, len);
        case USB_REQUEST_SET_INTERFACE:
            return usb_set_interface(dev, req, buf, len);
        case USB_REQUEST_GET_STATUS:
            return usb_get_status_interface(dev, req, buf, len);
	}

	return USB_RESULT_NOTSUPP;
}

static usb_result_t _usb_request_endpoint(usb_device_t *dev, usb_request_t *req, uint8_t **buf, uint16_t *len)
{
    switch (req->bRequest) {
        case USB_REQUEST_CLR_FEATURE:
            if (req->wValue == USB_FEATURE_ENDPOINT_HALT) {
                return _usb_endpoint_unstall(dev, req, buf, len);
            }
        case USB_REQUEST_SET_FEATURE:
            if (req->wValue == USB_FEATURE_ENDPOINT_HALT) {
                return _usb_endpoint_stall(dev, req, buf, len);
            }
        case USB_REQUEST_GET_STATUS:
            return usb_get_status_endpoint(dev, req, buf, len);
        case USB_REQUEST_SYNCH_FRAME:
            /* FIXME: SYNCH_FRAME is not implemented. */
            /*
            * SYNCH_FRAME is used for synchronization of isochronous
            * endpoints which are not yet implemented.
            */
            break;
	}

	return USB_RESULT_NOTSUPP;
}

__attribute__((used))
usb_result_t _usb_request(usb_device_t *dev, usb_request_t *req, uint8_t **buf, uint16_t *len)
{
    /* FIXME: Have class/vendor requests as well. */
	if ((req->bmRequestType & USB_REQ_TYPE_MASK) != USB_REQ_TYPE_STANDARD) {
		return USB_RESULT_NOTSUPP;
	}

	switch (req->bmRequestType & USB_REQ_RECIPIENT_MASK) {
        case USB_REQ_RECIPIENT_DEVICE:
            return _usb_request_device(dev, req, buf, len);
        case USB_REQ_RECIPIENT_INTERFACE:
            return _usb_request_interface(dev, req, buf, len);
        case USB_REQ_RECIPIENT_ENDPOINT:
            return _usb_request_endpoint(dev, req, buf, len);
	}
    
    return USB_RESULT_NOTSUPP;
}


//TODO all endpoint manipulation functions
__attribute__((weak))
void _usb_reset_endpoints(usb_device_t *dev) {
    (void) dev;
}