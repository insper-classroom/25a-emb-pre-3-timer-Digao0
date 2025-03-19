#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include <stdio.h>

const int BTN_PIN_R = 28;
const int LED_PIN_R = 4;

volatile int flag_f_r = 0;
volatile bool longo = false;
volatile bool apertado = false;
volatile alarm_id_t press_timer_id = 0;

int64_t press_timer_callback(alarm_id_t id, void *user_data) {
    if (apertado) {
        longo = true; 
    }
    return 0; 
}

void btn_callback(uint gpio, uint32_t events) {
    if (events == 0x4) { // fall edge
        flag_f_r = 1;
        apertado = true;
        longo = false; // Resetar estado
        press_timer_id = add_alarm_in_ms(500, press_timer_callback, NULL, false);

    } else if (events == 0x8) { // rise edge
        if (longo) {
            gpio_xor_mask(1 << LED_PIN_R); // altera led
        }
        apertado = false;
        longo = false; 
    }
}

int main() {
    stdio_init_all();

    gpio_init(LED_PIN_R);
    gpio_set_dir(LED_PIN_R, GPIO_OUT);

    gpio_init(BTN_PIN_R);
    gpio_set_dir(BTN_PIN_R, GPIO_IN);
    gpio_pull_up(BTN_PIN_R);

    gpio_set_irq_enabled_with_callback(
        BTN_PIN_R, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true, &btn_callback);

    while (true) {

        if (flag_f_r) {
            flag_f_r = 0;
        }
    }
}
