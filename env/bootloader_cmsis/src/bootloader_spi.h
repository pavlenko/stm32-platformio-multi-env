#ifndef __BOOTLOADER_SPI_H
#define __BOOTLOADER_SPI_H

#ifdef __cplusplus
extern "C" {
#endif

#define SPI_CMD_GET 0x00
#define SPI_CMD_GET_VERSION 0x01
#define SPI_CMD_GET_ID 0x02
#define SPI_CMD_READ_MEMORY 0x11
#define SPI_CMD_GO 0x21
#define SPI_CMD_WRITE_MEMORY 0x31
#define SPI_CMD_ERASE 0x44
#define SPI_CMD_SPECIAL 0x50
#define SPI_CMD_SPECIAL_EXTENDED 0x51
#define SPI_CMD_WRITE_PROTECT 0x63
#define SPI_CMD_WRITE_UNPROTECT 0x73
#define SPI_CMD_READOUT_PROTECT 0x82
#define SPI_CMD_READOUT_UNPROTECT 0x92
#define SPI_CMD_GET_CHECKSUM_NO_STRETCH 0xA1

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

#endif /* __BOOTLOADER_SPI_H */
