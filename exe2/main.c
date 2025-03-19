#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include <stdio.h>

const int BTN_PIN_R = 28;
const int BTN_PIN_G = 26;

const int LED_PIN_R = 4;
const int LED_PIN_G = 6;

volatile int flag_r = 0;
volatile int flag_g = 0;

void btn_callback(uint gpio, uint32_t events) {
    if (events == 0x4) {
        if (gpio == BTN_PIN_R)
            flag_r = 1;
        else if (gpio == BTN_PIN_G)
            flag_g = 1;
    }
}

bool timer_2_callback(repeating_timer_t *rt) {
    //led_state = !led_state;
    gpio_xor_mask(1 << LED_PIN_G);
    return true; //  timer rodando
}

bool timer_callback(repeating_timer_t *rt) {
    //led_state = !led_state;
    //gpio_put(LED_PIN_R, led_state);
    gpio_xor_mask(1 << LED_PIN_R);
    return true; //  timer rodando
}

int main() {
    gpio_init(LED_PIN_R);
    gpio_set_dir(LED_PIN_R, GPIO_OUT);

    gpio_init(LED_PIN_G);
    gpio_set_dir(LED_PIN_G, GPIO_OUT);

    gpio_init(BTN_PIN_R);
    gpio_set_dir(BTN_PIN_R, GPIO_IN);
    gpio_pull_up(BTN_PIN_R);

    gpio_init(BTN_PIN_G);
    gpio_set_dir(BTN_PIN_G, GPIO_IN);
    gpio_pull_up(BTN_PIN_G);

    gpio_set_irq_enabled_with_callback(BTN_PIN_R, GPIO_IRQ_EDGE_FALL, true,
                                       &btn_callback);
    gpio_set_irq_enabled(BTN_PIN_G, GPIO_IRQ_EDGE_FALL, true);

    repeating_timer_t timer2; //timer green
    bool timer_2_running = false; 

    repeating_timer_t timer; //timer red
    bool timer_running = false;      

    while (true) {

        if (flag_r) {
            flag_r = 0;
            if (timer_running){
            cancel_repeating_timer(&timer);
            gpio_put(LED_PIN_R, 0); // led termna apagado
            //led_state = 0;
            timer_running = false;
            } else{
                add_repeating_timer_ms(500, timer_callback, NULL, &timer);
                timer_running = true;
            }
        } 

        if (flag_g) {
            flag_g = 0;
            if (timer_2_running){
                cancel_repeating_timer(&timer2);
                gpio_put(LED_PIN_G, 0); // led termna apagado
                //led_state = 0;
                timer_2_running = false;
                } else{
                    add_repeating_timer_ms(250, timer_2_callback, NULL, &timer2);
                    timer_2_running = true;
                }
        }
    }
}
