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
    uint8_t opcode;
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
            //TODO create handler to command map...
    }
}
