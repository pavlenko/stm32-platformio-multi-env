#include <stdint.h>
#include <stm32f103x6.h>
#include <usbd.h>
#include <usb_dfu.h>

#include "usb.h"

extern usb_config_descriptor_t usb_config_descriptor;
extern usb_device_descriptor_t usb_device_descriptor;
extern const char *usb_strings[];

extern usb_driver_t st_usb_driver;

uint8_t usb_buffer[1024];
uint8_t dfu_buffer[1024];

//TODO create init function
dfu_t dfu = {
    .buf   = dfu_buffer,
    .state = DFU_STATE_DFU_IDLE,
};

//TODO create init function
usb_device_t usb_device = {
	.driver = &st_usb_driver,
    .device_descr = &usb_device_descriptor,
    .config_descr = &usb_config_descriptor,
    .strings = usb_strings,
    .num_strings = 4,
    .cb_control = {
        //{.cb = dfu_cb_control, .ptr = &dfu},
        dfu_cb_control,
    },
};

void dfu_system_reset()
{}

void dfu_memory_unlock()
{}

void dfu_memory_lock()
{}

void dfu_memory_erase_page(uint32_t address)
{
    (void) address;
}

void dfu_memory_write_uint16(uint32_t address, uint16_t data)
{
    (void) address;
    (void) data;
}

int main(void) {
    usbd_init(&usb_device);
    while (1) {
		st_usb_driver.poll(&usb_device);
    }
}
