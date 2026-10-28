/*
 * Copyright (c) 2016, Devan Lai
 *
 * Permission to use, copy, modify, and/or distribute this software
 * for any purpose with or without fee is hereby granted, provided
 * that the above copyright notice and this permission notice
 * appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
 * WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
 * AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,
 * NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>

#include "target.h"
#include "config.h"

/* Reconfigure processor settings */
void cpu_setup(void) {

}

/* Set STM32 to 72 MHz. */
void clock_setup(void) {
    // rcc_clock_setup_in_hse_8mhz_out_72mhz();
    rcc_clock_setup_pll(&rcc_hse_configs[RCC_CLOCK_HSE8_72MHZ]);
}

void gpio_setup(void) {
    /*
      jixin.pro CMSIS-DAP_LU のコピー品 (STM32F103 互換, 8 MHz HSE)。元ファームの GPIO を実測して割り当てた:
      TGT_SWCLK PA4, TGT_SWDIO PA2, TGT_RST PA6 (open-drain)
      UART TXD/RXD: USART1 PA9/PA10
      LED: PB8 (ピン 45) = CON, PB12 (ピン 25) = RUN/ACT。基板の配線をテスターで確認済み。極性は H 点灯と仮定
      PA8: 元ファームが起動時から出力 1。役割は未確認なので同じ状態にしておく
    */

    rcc_periph_clock_enable(RCC_GPIOA);
    rcc_periph_clock_enable(RCC_GPIOB);

    gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO8);
    gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO8 | GPIO12);
    gpio_set(GPIOA, GPIO8);
    gpio_clear(GPIOB, GPIO8 | GPIO12);
}

void target_console_init(void){
    /* Enable UART clock */
    rcc_periph_clock_enable(CONSOLE_USART_CLOCK);

    /* Setup GPIO pins */
    gpio_set_mode(CONSOLE_USART_GPIO_PORT, GPIO_MODE_OUTPUT_50_MHZ,
                  GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, CONSOLE_USART_GPIO_TX);
    gpio_set_mode(CONSOLE_USART_GPIO_PORT, GPIO_MODE_INPUT,
                  GPIO_CNF_INPUT_FLOAT, CONSOLE_USART_GPIO_RX);
}

void led_bit(uint8_t position, bool state) {
    uint32_t gpio = 0xFFFFFFFFU;
    if (position == 0) {
        gpio = GPIO8;
    } else if (position == 1) {
        gpio = GPIO12;
    }

    if (gpio != 0xFFFFFFFFU) {
        if (state ^ LED_OPEN_DRAIN) {
            gpio_set(GPIOB, gpio);
        } else {
            gpio_clear(GPIOB, gpio);
        }
    }
}

void led_num(uint8_t value) {
    led_bit(0, value & 0x1);
    led_bit(1, value & 0x2);
}
