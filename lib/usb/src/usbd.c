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
/* Exported functions --------------------------------------------------------*/

bool usb_register_cb_set_configuration(usb_device_t *dev, usb_cb_set_configuration_t cb)
{
    uint8_t i;
    for (i = 0; i < USB_MAX_CB_SET_CONFIGURATION; i++) {
        // if (dev->cb_control[i].cb != NULL) {
        //     continue;
        // }
        dev->cb_set_configuration[i] = cb;
        return true;
    }
    return false;
}

bool usb_register_cb_set_interface(usb_device_t *dev, usb_cb_set_interface_t cb)
{}

bool usb_register_cb_control(usb_device_t *dev, uint8_t type, uint8_t mask, usb_cb_control_t cb)
{}

/* Private functions ---------------------------------------------------------*/
