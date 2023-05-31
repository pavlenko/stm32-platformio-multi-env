#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/flash.h>
#include <libopencm3/cm3/scb.h>
#include <libopencm3/usb/usbd.h>
#include <libopencm3/usb/dfu.h>

#include "config.h"

#define APP_ADDRESS (FLASH_BASE + DFU_SIZE)

int main(void) {
    usbd_device *dev;

    // Setup GPIO
    rcc_periph_clock_enable(RCC_GPIOA);
    gpio_set_mode(DFU_CHECK_GPIO_PORT, GPIO_MODE_INPUT, GPIO_CNF_INPUT_PULL_UPDOWN, DFU_CHECK_GPIO_PIN);
    gpio_clear(DFU_CHECK_GPIO_PORT, DFU_CHECK_GPIO_PIN);
    for (uint16_t i = 0; i < 512; i++) {
        __asm__("nop");
    }

    // Check GPIO
    if (!gpio_get(DFU_CHECK_GPIO_PORT, DFU_CHECK_GPIO_PIN)) {
        // Boot the application if it's valid.
        if ((*(volatile uint32_t *)APP_ADDRESS & 0x2FFE0000) == 0x20000000) {
            // Set vector table base address.
            SCB_VTOR = APP_ADDRESS & 0xFFFF;
            // Initialise master stack pointer.
            asm volatile("msr msp, %0"::"g" (*(volatile uint32_t *)APP_ADDRESS));
            // Jump to application.
            (*(void (**)(void))(APP_ADDRESS + 4))();
        }
    }

    // Setup bootloader
    rcc_clock_setup_in_hse_8mhz_out_72mhz();

    //TODO here

    // USB init start
    rcc_periph_clock_enable(RCC_USB);
    // USB init end

    while (1) {
        
    }
}
