#ifndef __BOOTLOADER_UART_H
#define __BOOTLOADER_UART_H

#ifdef __cplusplus
extern "C" {
#endif

#define UART_CMD_GET 0x00
#define UART_CMD_GET_VERSION 0x01
#define UART_CMD_GET_ID 0x02
#define UART_CMD_READ_MEMORY 0x11
#define UART_CMD_GO 0x21
#define UART_CMD_WRITE_MEMORY 0x31
#define UART_CMD_ERASE_MEMORY 0x43
#define UART_CMD_ERASE_MEMORY_EXTENDED 0x44
#define UART_CMD_SPECIAL 0x50
#define UART_CMD_SPECIAL_EXTENDED 0x51
#define UART_CMD_WRITE_PROTECT 0x63
#define UART_CMD_WRITE_UNPROTECT 0x73
#define UART_CMD_READOUT_PROTECT 0x82
#define UART_CMD_READOUT_UNPROTECT 0x92
#define UART_CMD_GET_CHECKSUM 0xA1

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

#endif /* __BOOTLOADER_UART_H */
