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
//TODO rename props (un)protect?
typedef struct {
    void (*get_command)(void);
    void (*get_version)(void);
    void (*get_id)(void);
    void (*speed)(void);
    void (*read_memory)(void);
    void (*go)(void);
    void (*write_memory)(void);
    void (*write_memory_ns)(void);
    void (*erase_memory)(void);
    void (*erase_memory_ns)(void);
    void (*special)(void);
    void (*special_extended)(void);
    void (*write_protect)(void);
    void (*write_protect_ns)(void);
    void (*write_unprotect)(void);
    void (*write_unprotect_ns)(void);
    void (*readout_protect)(void);
    void (*readout_protect_ns)(void);
    void (*readout_unprotect)(void);
    void (*readout_unprotect_ns)(void);
} bootloader_commands_t;

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions prototypes ---------------------------------------------*/
/* Private defines -----------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* __BOOTLOADER_H */
