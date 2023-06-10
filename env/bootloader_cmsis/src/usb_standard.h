#ifndef __USB_STANDARD_H
#define __USB_STANDARD_H

#include <stdint.h>

#define USB_DESCRIPTOR_TYPE_DEVICE     0x01
#define USB_DESCRIPTOR_TYPE_CONFIG     0x02
#define USB_DESCRIPTOR_TYPE_STRING     0x03
#define USB_DESCRIPTOR_TYPE_INTERFACE  0x04
#define USB_DESCRIPTOR_TYPE_ENDPOINT   0x05

#define USB_REQUEST_GET_STATUS        0x00
#define USB_REQUEST_CLR_FEATURE       0x01
#define USB_REQUEST_SET_FEATURE       0x03
#define USB_REQUEST_SET_ADDRESS       0x05
#define USB_REQUEST_GET_DESCRIPTOR    0x06
#define USB_REQUEST_SET_DESCRIPTOR    0x07
#define USB_REQUEST_GET_CONFIGURATION 0x08
#define USB_REQUEST_SET_CONFIGURATION 0x09
#define USB_REQUEST_GET_INTERFACE     0x0A
#define USB_REQUEST_SET_INTERFACE     0x0B
#define USB_REQUEST_SYNCH_FRAME       0x0C

#define USB_REQ_TYPE_STANDARD  0x1F
#define USB_REQ_TYPE_RECIPIENT 0x60

#define USB_REQ_TYPE_RECIPIENT_DEVICE    0x00
#define USB_REQ_TYPE_RECIPIENT_INTERFACE 0x01
#define USB_REQ_TYPE_RECIPIENT_ENDPOINT  0x02
#define USB_REQ_TYPE_RECIPIENT_OTHER     0x03

typedef struct {
    uint8_t bmRequestType;
    //TODO descriptors, endpoints, etc
} usb_device_t;

void usb_get_status(usb_device_t *dev);
void usb_get_configuration();
void usb_set_configuration();
void usb_get_interface();
void usb_set_interface();

#endif //__USB_STANDARD_H
