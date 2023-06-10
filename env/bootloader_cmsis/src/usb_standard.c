#include "usb_standard.h"

void usb_get_status(usb_device_t *dev) {
    uint8_t recipient = dev->bmRequestType & (USB_REQ_TYPE_STANDARD | USB_REQ_TYPE_RECIPIENT);
    if (recipient == USB_REQ_TYPE_RECIPIENT_DEVICE) {
        //TODO status
    }
    if (recipient == USB_REQ_TYPE_RECIPIENT_INTERFACE) {
        //TODO status
    }
    if (recipient == USB_REQ_TYPE_RECIPIENT_ENDPOINT) {
        //TODO status
    }
}