#ifndef __FLASH_CONFIG_H
#define __FLASH_CONFIG_H

#include "stm32f103xb.h"

#ifndef VERSION
#define VERSION "v1.1.0"
#endif

// Bootloader size in kb, 4 by default
#ifndef BOOTLOADER_SIZE
#define BOOTLOADER_SIZE 4UL
#endif

// Define flash size in kb, 128 by default
#ifndef FLASH_SIZE
#define FLASH_SIZE 128UL
#endif

#ifndef SRAM_SIZE
#define SRAM_SIZE (20480 - 8)
#endif

#define FLASH_BASE_ADDR 0x08000000
#define FLASH_BOOTLDR_PAYLOAD_SIZE_KB (FLASH_SIZE - BOOTLOADER_SIZE)

// Default config
#define WINUSB_SUPPORT
#define ENABLE_CHECKSUM
#define ENABLE_WATCHDOG 20

// For GPIO DFU booting: -DENABLE_GPIO_DFU_BOOT -DGPIO_DFU_BOOT_PORT=GPIOB -DGPIO_DFU_BOOT_PIN=2
#define ENABLE_GPIO_DFU_BOOT
#define GPIO_DFU_BOOT_PORT GPIOB_BASE //TODO port
#define GPIO_DFU_BOOT_PIN 2

// To protect bootloader from accidental writes: -DENABLE_WRITEPROT
#define ENABLE_WRITEPROT

// To protect your payload from DFU reads: -DENABLE_SAFEWRITE
#ifndef ENABLE_SAFEWRITE
#define ENABLE_SAFEWRITE
#endif

#ifndef USB_VID
#define USB_VID 0xDEAD
#endif

#ifndef USB_PID
#define USB_PID 0xCA5D
#endif

//#define FLASH_BASE_ADDR $(FLASH_BASE_ADDR)
#define FLASH_SIZE_KB FLASH_SIZE
//#define FLASH_BOOTLDR_PAYLOAD_SIZE_KB $(FLASH_BOOTLDR_PAYLOAD_SIZE_KB)
#define FLASH_BOOTLDR_SIZE_KB BOOTLOADER_SIZE

#endif //__FLASH_CONFIG_H
