#ifndef __BOOTLOADER_I2C_H
#define __BOOTLOADER_I2C_H

#ifdef __cplusplus
extern "C" {
#endif

#define I2C_CMD_GET 0x00
#define I2C_CMD_GET_VERSION 0x01
#define I2C_CMD_GET_ID 0x02
#define I2C_CMD_READ_MEMORY 0x11
#define I2C_CMD_GO 0x21
#define I2C_CMD_WRITE_MEMORY 0x31
#define I2C_CMD_WRITE_MEMORY_NO_STRETCH 0x32
#define I2C_CMD_ERASE 0x44
#define I2C_CMD_ERASE_NO_STRETCH 0x45
#define I2C_CMD_SPECIAL 0x50
#define I2C_CMD_SPECIAL_EXTENDED 0x51
#define I2C_CMD_WRITE_PROTECT 0x63
#define I2C_CMD_WRITE_PROTECT_NO_STRETCH 0x64
#define I2C_CMD_WRITE_UNPROTECT 0x73
#define I2C_CMD_WRITE_UNPROTECT_NO_STRETCH 0x74
#define I2C_CMD_READOUT_PROTECT 0x82
#define I2C_CMD_READOUT_PROTECT_NO_STRETCH 0x83
#define I2C_CMD_READOUT_UNPROTECT 0x92
#define I2C_CMD_READOUT_UNPROTECT_NO_STRETCH 0x93
#define I2C_CMD_GET_CHECKSUM_NO_STRETCH 0xA1

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

#endif /* __BOOTLOADER_I2C_H */
