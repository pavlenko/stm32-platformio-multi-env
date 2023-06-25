#ifndef __USB_CONTROL_H
#define __USB_CONTROL_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum usb_state_e {
    USB_STATE_IDLE,
    USB_STATE_STALLED,
    USB_STATE_DATA_IN,
    USB_STATE_LAST_DATA_IN,
    USB_STATE_DATA_OUT,
    USB_STATE_LAST_DATA_OUT,
    USB_STATE_STATUS_IN,
    USB_STATE_STATUS_OUT,
} usb_state_t;

#ifdef __cplusplus
}
#endif

#endif //__USB_CONTROL_H