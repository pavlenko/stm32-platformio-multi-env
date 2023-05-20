/* Includes ------------------------------------------------------------------*/
#include "stddef.h"
#include "bootloader.h"

/* Private includes ----------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private user code ---------------------------------------------------------*/

void bootloader_dispatch(bootloader_interface_t *interface)
{
    if (interface->api->get_opcode == NULL) {
        return;
    }

    uint8_t opcode = interface->api->get_opcode();
    switch (opcode) {
        case CMD_GET_COMMAND:
            if (interface->cmd->get_command != NULL) {
                interface->cmd->get_command();
            } else if (interface->api->send_byte != NULL) {
                interface->api->send_byte(NACK_BYTE);
            }
            break;
        case CMD_GET_VERSION:
            if (interface->cmd->get_version != NULL) {
                interface->cmd->get_version();
            } else if (interface->api->send_byte != NULL) {
                interface->api->send_byte(NACK_BYTE);
            }
            break;
        case CMD_GET_ID:
            if (interface->cmd->get_id != NULL) {
                interface->cmd->get_id();
            } else if (interface->api->send_byte != NULL) {
                interface->api->send_byte(NACK_BYTE);
            }
            break;
        case CMD_SPEED:
            if (interface->cmd->speed != NULL) {
                interface->cmd->speed();
            } else if (interface->api->send_byte != NULL) {
                interface->api->send_byte(NACK_BYTE);
            }
            break;
        case CMD_READ_MEMORY:
            if (interface->cmd->read_memory != NULL) {
                interface->cmd->read_memory();
            } else if (interface->api->send_byte != NULL) {
                interface->api->send_byte(NACK_BYTE);
            }
            break;
        case CMD_GO:
            if (interface->cmd->go != NULL) {
                interface->cmd->go();
            } else if (interface->api->send_byte != NULL) {
                interface->api->send_byte(NACK_BYTE);
            }
            break;
        case CMD_WRITE_MEMORY:
            if (interface->cmd->write_memory != NULL) {
                interface->cmd->write_memory();
            } else if (interface->api->send_byte != NULL) {
                interface->api->send_byte(NACK_BYTE);
            }
            break;
        case CMD_WRITE_MEMORY_NS:
            if (interface->cmd->write_memory_ns != NULL) {
                interface->cmd->write_memory_ns();
            } else if (interface->api->send_byte != NULL) {
                interface->api->send_byte(NACK_BYTE);
            }
            break;
        case CMD_ERASE_MEMORY_LEGACY:
        case CMD_ERASE_MEMORY:
            if (interface->cmd->erase_memory != NULL) {
                interface->cmd->erase_memory();
            } else if (interface->api->send_byte != NULL) {
                interface->api->send_byte(NACK_BYTE);
            }
            break;
        case CMD_ERASE_MEMORY_NS:
            if (interface->cmd->erase_memory_ns != NULL) {
                interface->cmd->erase_memory_ns();
            } else if (interface->api->send_byte != NULL) {
                interface->api->send_byte(NACK_BYTE);
            }
            break;
        case CMD_SPECIAL:
            if (interface->cmd->special != NULL) {
                interface->cmd->special();
            } else if (interface->api->send_byte != NULL) {
                interface->api->send_byte(NACK_BYTE);
            }
            break;
        case CMD_SPECIAL_EXTENDED:
            if (interface->cmd->special_extended != NULL) {
                interface->cmd->special_extended();
            } else if (interface->api->send_byte != NULL) {
                interface->api->send_byte(NACK_BYTE);
            }
            break;
        case CMD_WRITE_PROTECT:
            if (interface->cmd->write_protect != NULL) {
                interface->cmd->write_protect();
            } else if (interface->api->send_byte != NULL) {
                interface->api->send_byte(NACK_BYTE);
            }
            break;
        case CMD_WRITE_PROTECT_NS:
            if (interface->cmd->write_protect_ns != NULL) {
                interface->cmd->write_protect_ns();
            } else if (interface->api->send_byte != NULL) {
                interface->api->send_byte(NACK_BYTE);
            }
            break;
        case CMD_WRITE_UNPROTECT:
            if (interface->cmd->write_unprotect != NULL) {
                interface->cmd->write_unprotect();
            } else if (interface->api->send_byte != NULL) {
                interface->api->send_byte(NACK_BYTE);
            }
            break;
        case CMD_WRITE_UNPROTECT_NS:
            if (interface->cmd->write_unprotect_ns != NULL) {
                interface->cmd->write_unprotect_ns();
            } else if (interface->api->send_byte != NULL) {
                interface->api->send_byte(NACK_BYTE);
            }
            break;
        case CMD_READOUT_PROTECT:
            if (interface->cmd->readout_protect != NULL) {
                interface->cmd->readout_protect();
            } else if (interface->api->send_byte != NULL) {
                interface->api->send_byte(NACK_BYTE);
            }
            break;
        case CMD_READOUT_PROTECT_NS:
            if (interface->cmd->readout_protect_ns != NULL) {
                interface->cmd->readout_protect_ns();
            } else if (interface->api->send_byte != NULL) {
                interface->api->send_byte(NACK_BYTE);
            }
            break;
        case CMD_READOUT_UNPROTECT:
            if (interface->cmd->readout_unprotect != NULL) {
                interface->cmd->readout_unprotect();
            } else if (interface->api->send_byte != NULL) {
                interface->api->send_byte(NACK_BYTE);
            }
            break;
        case CMD_READOUT_UNPROTECT_NS:
            if (interface->cmd->readout_unprotect_ns != NULL) {
                interface->cmd->readout_unprotect_ns();
            } else if (interface->api->send_byte != NULL) {
                interface->api->send_byte(NACK_BYTE);
            }
            break;
        default:
            if (interface->api->send_byte != NULL) {
                interface->api->send_byte(NACK_BYTE);
            }
            break;
    }
}
