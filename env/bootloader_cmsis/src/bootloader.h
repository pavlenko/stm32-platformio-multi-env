#ifndef __BOOTLOADER_H
#define __BOOTLOADER_H

#ifdef __cplusplus
extern "C" {
#endif

// Based on STM open-bootloader project
#define CMD_GET_COMMAND             0x00U
#define CMD_GET_VERSION             0x01U
#define CMD_GET_ID                  0x02U
#define CMD_SPEED                   0x03U
#define CMD_READ_MEMORY             0x11U
#define CMD_GO                      0x21U
#define CMD_WRITE_MEMORY            0x31U
#define CMD_WRITE_MEMORY_NS         0x32U // no-stretch, just known
#define CMD_ERASE_MEMORY_LEGACY     0x43U
#define CMD_ERASE_MEMORY            0x44U
#define CMD_ERASE_MEMORY_NS         0x45U // no-stretch, just known
#define CMD_SPECIAL                 0x50U
#define CMD_SPECIAL_EXTENDED        0x51U
#define CMD_WRITE_PROTECT           0x63U
#define CMD_WRITE_PROTECT_NS        0x64U // no-stretch, just known
#define CMD_WRITE_UNPROTECT         0x73U
#define CMD_WRITE_UNPROTECT_NS      0x74U // no-stretch, just known
#define CMD_READOUT_PROTECT         0x82U
#define CMD_READOUT_PROTECT_NS      0x83U // no-stretch, just known
#define CMD_READOUT_UNPROTECT       0x92U
#define CMD_READOUT_UNPROTECT_NS    0x93U // no-stretch, just known
#define CMD_GET_CHECKSUM            0xA1U

/* Includes ------------------------------------------------------------------*/
/* Private includes ----------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions prototypes ---------------------------------------------*/
/* Private defines -----------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* __BOOTLOADER_H */
