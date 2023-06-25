#include "usb_dfu.h"

usb_result_t dfu_control_request(usb_device_t *dev, usb_request_t *req, uint8_t **buf, uint16_t *len)
{
    //TODO
    (void) dev;
    (void) req;
    (void) buf;
    (void) len;
    return USB_RESULT_HANDLED;
}