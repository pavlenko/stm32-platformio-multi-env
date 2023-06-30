#include "usbd.h"

/* Includes ------------------------------------------------------------------*/
#include <stddef.h>
#include "usb_private.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Extern function prototypes ------------------------------------------------*/
/* Function definitions ------------------------------------------------------*/

void usbd_init(usb_device_t *dev)
{
    //TODO init HW
    dev->cb_endpoint[0][USB_TRANSACTION_SETUP] = usb_control_setup;
	dev->cb_endpoint[0][USB_TRANSACTION_OUT]   = usb_control_out;
	dev->cb_endpoint[0][USB_TRANSACTION_IN]    = usb_control_in;

	uint8_t i;
	for (i = 0; i < USB_MAX_CB_SET_CONFIGURATION; i++) {
		dev->cb_set_configuration[i] = NULL;
	}
}

bool usb_register_cb_set_configuration(usb_device_t *dev, usb_cb_set_configuration_t cb)
{
    uint8_t i;
    for (i = 0; i < USB_MAX_CB_SET_CONFIGURATION; i++) {
        if (dev->cb_set_configuration[i] == cb) {
            return true;
        }
        if (dev->cb_set_configuration[i] != NULL) {
            continue;
        }
        dev->cb_set_configuration[i] = cb;
        return true;
    }
    return false;
}

bool usb_register_cb_set_interface(usb_device_t *dev, usb_cb_set_interface_t cb)
{
    //TODO
    (void) dev;
    (void) cb;
    return true;
}

bool usb_register_cb_control(usb_device_t *dev, uint8_t type, uint8_t mask, usb_cb_control_t cb)
{
    //TODO
    (void) dev;
    (void) type;
    (void) mask;
    (void) cb;
    return true;
}

void usb_reset(usb_device_t *dev)
{
	dev->current_address = 0;
	dev->current_config = 0;
	usb_ep_setup(dev, 0, USB_ENDPOINT_TRANSFER_TYPE_CONTROL, dev->device_descr->bMaxPacketSize0, NULL);
	usb_set_address(dev, 0);

	// if (usbd_dev->user_callback_reset) {
	// 	usbd_dev->user_callback_reset();
	// }
}
