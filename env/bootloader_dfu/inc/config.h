#ifndef __CONFIG_H
#define __CONFIG_H

#include <libopencm3/stm32/gpio.h>

#ifndef DFU_KEY
#define DFU_KEY 0x544F4F42u
#endif

#ifndef DFU_TRANSFER_SIZE
#define DFU_TRANSFER_SIZE 1024u
#endif

#ifndef DFU_VERSION
#define DFU_VERSION "v1.1"
#endif

#ifndef DFU_SIZE
#define DFU_SIZE 4096u
#endif

#ifndef FLASH_SIZE
#define FLASH_SIZE 65536u // 64k by default, depends on cpu
#endif

#ifndef DFU_CHECK_GPIO_DISABLED

#ifndef DFU_CHECK_GPIO_PORT
#define DFU_CHECK_GPIO_PORT GPIOB
#endif

#ifndef DFU_CHECK_GPIO_PIN
#define DFU_CHECK_GPIO_PIN GPIO2
#endif

#endif

#ifndef USB_VID
#define USB_VID 0x1209u
#endif

#ifndef USB_PID
#define USB_PID 0xDB42u
#endif

#ifndef USB_VENDOR_STRING
#define USB_VENDOR_STRING "JOHN DOE"
#endif

#ifndef USB_PRODUCT_STRING
#define USB_PRODUCT_STRING "DFU Bootloader"
#endif

#ifndef USB_INTERFACE_STRING
#define USB_INTERFACE_STRING "DFU"
#endif

#endif //__CONFIG_H
