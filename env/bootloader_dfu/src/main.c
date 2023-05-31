#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>

#include "config.h"

#define APP_ADDRESS (FLASH_BASE + DFU_SIZE)

// Enables DFU mode when a reset from the nRST pin occurs.
static inline uint32_t reset_due_to_pin(void) {
    return (RCC_CSR & RCC_CSR_PINRSTF)
           && !(RCC_CSR & (RCC_CSR_LPWRRSTF | RCC_CSR_WWDGRSTF | RCC_CSR_IWDGRSTF | RCC_CSR_SFTRSTF | RCC_CSR_PORRSTF));
}

// If watchdog trigger reset.
static inline uint32_t reset_due_to_watchdog(void) {
    return (RCC_CSR & RCC_CSR_IWDGRSTF);
}

int main(void) {
    //TODO maybe check RTC backup registers instead of ram (do not need ram remap)
    //TODO or check if: last 8 RAM bytes = DFU_KEY - got to dfu
    //TODO or check if: reset_due_to_pin
    //TODO or check if: reset_due_to_watchdog
    //TODO or check if: invalid checksum

#ifndef DFU_CHECK_GPIO_DISABLED
    // check force_dfu_gpio
    rcc_periph_clock_enable(RCC_GPIOA);
    gpio_set_mode(DFU_CHECK_GPIO_PORT, GPIO_MODE_INPUT, GPIO_CNF_INPUT_PULL_UPDOWN, DFU_CHECK_GPIO_PIN);
    gpio_clear(DFU_CHECK_GPIO_PORT, DFU_CHECK_GPIO_PIN);
    for (uint16_t i = 0; i < 512; i++) {
        __asm__("nop");
    }
    bool go_dfu = gpio_get(DFU_CHECK_GPIO_PORT, DFU_CHECK_GPIO_PIN);
    gpio_set_mode(DFU_CHECK_GPIO_PORT, GPIO_MODE_INPUT, GPIO_CNF_INPUT_FLOAT, DFU_CHECK_GPIO_PIN);
#endif

    if (!go_dfu) {
        // Set vector table base address.
        volatile uint32_t *_csb_vtor = (uint32_t*)0xE000ED08U;
        *_csb_vtor = APP_ADDRESS & 0xFFFF;

        // Initialise master stack pointer.
        __asm__ volatile("msr msp, %0"::"g" (*(volatile uint32_t *)APP_ADDRESS));

        // Jump to application.
        (*(void (**)(void))(APP_ADDRESS + 4))();
    }

    rcc_clock_setup_in_hse_8mhz_out_72mhz();

    while (1) {
        
    }
}
