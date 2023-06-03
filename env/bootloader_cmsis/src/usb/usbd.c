#include "stdint.h"
#include "stm32f1xx.h"
#include "usbd.h"

#define IMR_MSK (CNTR_CTRM  | CNTR_WKUPM | CNTR_SUSPM | CNTR_ERRM  | CNTR_SOFM \
                 | CNTR_ESOFM | CNTR_RESETM )

__IO uint16_t wIstr;  /* ISTR register last read value */

void usb_irq_handler(void) {
    if (USB->ISTR & USB_ISTR_CTR) {
        //TODO (void)PCD_EP_ISR_Handler(hpcd);
    }

    if (USB->ISTR & USB_ISTR_RESET) {
        USB->ISTR &= (uint16_t) ~USB_ISTR_RESET;
        //TODO (void)HAL_PCD_SetAddress(hpcd, 0U);
    }

    if (USB->ISTR & USB_ISTR_PMAOVR) {
        USB->ISTR &= (uint16_t) ~USB_ISTR_PMAOVR;
    }

    if (USB->ISTR & USB_ISTR_ERR) {
        USB->ISTR &= (uint16_t) ~USB_ISTR_ERR;
    }

    if (USB->ISTR & USB_ISTR_WKUP) {
        USB->CNTR &= (uint16_t) ~USB_CNTR_LP_MODE;
        USB->CNTR &= (uint16_t) ~USB_CNTR_FSUSP;

        //TODO HAL_PCD_ResumeCallback(hpcd);

        USB->ISTR &= (uint16_t) ~USB_ISTR_WKUP;
    }

    if (USB->ISTR & USB_ISTR_SUSP) {
        /* WA: To Clear Wakeup flag if raised with suspend signal */

        uint8_t i, store_ep[8];
        for (i = 0U; i < 8U; i++) {
            store_ep[i] = PCD_GET_ENDPOINT(hpcd->Instance, i);/* Store Endpoint register */
        }

        USB->CNTR |= (uint16_t) USB_CNTR_FRES;/* FORCE RESET */
        USB->CNTR &= (uint16_t) ~USB_CNTR_FRES;/* CLEAR RESET */

        while ((USB->ISTR & USB_ISTR_RESET) == 0U);/* wait for reset flag in ISTR */

        USB->ISTR &= (uint16_t) USB_ISTR_RESET;/* Clear Reset Flag */

        for (i = 0U; i < 8U; i++) {
            PCD_SET_ENDPOINT(hpcd->Instance, i, store_ep[i]);/* Restore Endpoint register */
        }

        USB->CNTR |= (uint16_t) USB_CNTR_FSUSP;/* Force low-power mode in the macro cell */
        USB->ISTR &= (uint16_t) ~USB_ISTR_SUSP;/* clear of the ISTR bit must be done after setting of CNTR_FSUSP */
        USB->CNTR |= (uint16_t) USB_CNTR_LP_MODE;

        //TODO HAL_PCD_SuspendCallback(hpcd);
    }

#if (IMR_MSK & ISTR_SOF)
    if (wIstr & ISTR_SOF & wInterrupt_Mask)
  {
    _SetISTR((uint16_t)CLR_SOF);
    bIntPackSOF++;

#ifdef SOF_CALLBACK
    SOF_Callback();
#endif
  }
#endif
    /*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/
#if (IMR_MSK & ISTR_DOVR)
    if (wIstr & ISTR_DOVR & wInterrupt_Mask)
  {
    _SetISTR((uint16_t)CLR_DOVR);
#ifdef DOVR_CALLBACK
    DOVR_Callback();
#endif
  }
#endif
    /*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/
#if (IMR_MSK & ISTR_ERR)
    if (wIstr & ISTR_ERR & wInterrupt_Mask)
  {
    _SetISTR((uint16_t)CLR_ERR);
#ifdef ERR_CALLBACK
    ERR_Callback();
#endif
  }
#endif
    /*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/

#if (IMR_MSK & ISTR_ESOF)
    if (wIstr & ISTR_ESOF & wInterrupt_Mask)
  {
    /* clear ESOF flag in ISTR */
    _SetISTR((uint16_t)CLR_ESOF);

    if ((_GetFNR()&FNR_RXDP)!=0)
    {
      /* increment ESOF counter */
      esof_counter ++;

      /* test if we enter in ESOF more than 3 times with FSUSP =0 and RXDP =1=>> possible missing SUSP flag*/
      if ((esof_counter >3)&&((_GetCNTR()&CNTR_FSUSP)==0))
      {
        /* this a sequence to apply a force RESET*/

        /*Store CNTR value */
        wCNTR = _GetCNTR();

        /*Store endpoints registers status */
        for (i=0;i<8;i++) EP[i] = _GetENDPOINT(i);

        /*apply FRES */
        wCNTR|=CNTR_FRES;
        _SetCNTR(wCNTR);

        /*clear FRES*/
        wCNTR&=~CNTR_FRES;
        _SetCNTR(wCNTR);

        /*poll for RESET flag in ISTR*/
        while((_GetISTR()&ISTR_RESET) == 0);

        /* clear RESET flag in ISTR */
        _SetISTR((uint16_t)CLR_RESET);

       /*restore Enpoints*/
        for (i=0;i<8;i++)
        _SetENDPOINT(i, EP[i]);

        esof_counter = 0;
      }
    }
    else
    {
        esof_counter = 0;
    }

    /* resume handling timing is made with ESOFs */
    Resume(RESUME_ESOF); /* request without change of the machine state */

#ifdef ESOF_CALLBACK
    ESOF_Callback();
#endif
  }
#endif
}