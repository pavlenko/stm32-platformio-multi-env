#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>

#include "config.h"

#define APP_BASE (FLASH_BASE + DFU_SIZE)

// Enables DFU mode when a reset from the nRST pin occurs.
static inline uint32_t reset_due_to_pin(void) {
    return (RCC_CSR & RCC_CSR_PINRSTF)
           && !(RCC_CSR & (RCC_CSR_LPWRRSTF | RCC_CSR_WWDGRSTF | RCC_CSR_IWDGRSTF | RCC_CSR_SFTRSTF | RCC_CSR_PORRSTF));
}

// If watchdog trigger reset.
static inline uint32_t reset_due_to_watchdog(void) {
    return (RCC_CSR & RCC_CSR_IWDGRSTF);
}

// If some pin has some state
static int force_dfu_gpio(void) {
	rcc_gpio_enable(GPIO_DFU_BOOT_PORT);
	gpio_set_input_pp(GPIO_DFU_BOOT_PORT, GPIO_DFU_BOOT_PIN);
	gpio_clear(GPIO_DFU_BOOT_PORT, GPIO_DFU_BOOT_PIN);
	for (unsigned int i = 0; i < 512; i++)
		__asm__("nop");
	uint16_t val = gpio_read(GPIO_DFU_BOOT_PORT, GPIO_DFU_BOOT_PIN);
	gpio_set_input(GPIO_DFU_BOOT_PORT, GPIO_DFU_BOOT_PIN);
	return val != 0;
}

int main(void) {
    rcc_clock_setup_in_hse_8mhz_out_72mhz();

    //TODO maybe check RTC backup registers instead of ram (do not need ram remap)
    //TODO or check if: last 8 RAM bytes = DFU_KEY - got to dfu
    //TODO or check if: reset_due_to_pin
    //TODO or check if: reset_due_to_watchdog
    //TODO or check if: force_dfu_gpio
    //TODO or check if: invalid checksum

    while (1) {
        
    }
}
