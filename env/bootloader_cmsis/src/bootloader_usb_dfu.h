#ifndef __BOOTLOADER_USB_DFU_H
#define __BOOTLOADER_USB_DFU_H

#ifdef __cplusplus
extern "C" {
#endif

#define USB_DFU_REQUEST_DETACH 0x00
#define USB_DFU_REQUEST_DNLOAD 0x01
#define USB_DFU_REQUEST_UPLOAD 0x02
#define USB_DFU_REQUEST_GETSTATUS 0x03
#define USB_DFU_REQUEST_CLRSTATUS 0x04
#define USB_DFU_REQUEST_GETSTATE 0x05
#define USB_DFU_REQUEST_ABORT 0x06

#define USB_DFU_CMD_GET 0x00
#define USB_DFU_CMD_SET_ADDRESS_POINTER 0x21
#define USB_DFU_CMD_ERASE 0x41
#define USB_DFU_CMD_READ_UNPROTECT 0x92


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

#endif /* __BOOTLOADER_USB_DFU_H */
