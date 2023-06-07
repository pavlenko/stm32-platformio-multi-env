#ifndef __USB_H
#define __USB_H

#ifdef __cplusplus
extern "C" {
#endif

__attribute__((used))
void USB_LP_CAN1_RX0_IRQHandler(void);

void usb_irq_handler(void);

#ifdef __cplusplus
}
#endif

#endif //__USB_H
