#ifndef __CONFIG_H
#define __CONFIG_H

// API configuration
//TODO allow possibility to store key in ram OR rtc backup registers (if registers used by user app???)
#define DFU_KEY 0x544F4F42u

#ifndef DFU_VERSION
#define DFU_VERSION "v1.1"
#endif

#ifndef DFU_SIZE
#define DFU_SIZE (4 * 1024)
#endif

#ifndef FLASH_SIZE
#define FLASH_SIZE 65536 // 64k by default, depends on cpu
#endif

// USB configuration
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
