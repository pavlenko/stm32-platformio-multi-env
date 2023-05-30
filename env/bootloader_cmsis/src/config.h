#ifndef STM32_PLATFORMIO_MULTI_ENV_CONFIG_H
#define STM32_PLATFORMIO_MULTI_ENV_CONFIG_H

/* USB VID */
#ifndef DFU_VENDOR_ID
#define DFU_VENDOR_ID       0x0483
#endif

/* USB PID */
#ifndef DFU_DEVICE_ID
#define DFU_DEVICE_ID       0xDF11
#endif

/* USB manufacturer string */
#ifndef DFU_STR_MANUF
#define DFU_STR_MANUF       "Your company name"
#endif
/* USB product sting */
#ifndef DFU_STR_PRODUCT
#define DFU_STR_PRODUCT     "Secure bootloader"
#endif
/* USB string for DFU configureation string descriptor. */
#ifndef DFU_DSC_CONFIG
#define DFU_DSC_CONFIG      _ENABLE
#endif
#ifndef DFU_STR_CONFIG
#define DFU_STR_CONFIG      "DFU"
#endif
/* USB string for DFU flash interface string descriptor. */
#ifndef DFU_DSC_FLASH
#define DFU_DSC_FLASH       _ENABLE
#endif
#ifndef DFU_STR_FLASH
#define DFU_STR_FLASH       "Internal flash"
#endif
/* USB string for DFU EEPROM interface string descriptor */
#ifndef DFU_DSC_EEPROM
#define DFU_DSC_EEPROM      _ENABLE
#endif

#ifndef DFU_STR_EEPROM
#define DFU_STR_EEPROM       "Internal EEPROM"
#endif

/* USB EP0 size. Must be 8 for USB FS */
#define DFU_EP0_SIZE        8

/* DFU properties */
#ifndef DFU_POLL_TIMEOUT
#define DFU_POLL_TIMEOUT    20
#endif
#ifndef DFU_DETACH_TIMEOUT
#define DFU_DETACH_TIMEOUT  200
#endif
#ifndef DFU_BLOCK_SIZE
#define DFU_BLOCK_SIZE         0x80
#endif

#define DFU_BUFFER_SIZE    ((DFU_BLOCK_SIZE + 3 + 8) >> 2)

#endif //STM32_PLATFORMIO_MULTI_ENV_CONFIG_H
