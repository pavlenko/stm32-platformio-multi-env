#ifndef __USB_DFU_H
#define __USB_DFU_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"

typedef struct {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bmAttributes;
    uint16_t wDetachTimeout;
    uint16_t wTransferSize;
    uint16_t bcdDFUVersion;
} __attribute__((packed)) usb_dfu_descriptor_t;

#ifdef __cplusplus
}
#endif

#endif //__USB_DFU_H
