#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/xosc.h"
#include "pico/multicore.h"
#include "functions.h"

int moles[] = {3,5,7,9,11};
int lights[] = {4,6,8,10,12};
int pressed_mole;
bool hit;

void gpio_callback(uint gpio, uint32_t events) { //tell us what button is pressed
    if (events & GPIO_IRQ_EDGE_RISE) {
        pressed_mole = gpio;
        hit = true;
    }
}

void init_gpio() {

    for(int i = 0; i < 5; i++) {
        gpio_init(moles[i]); //set moles as inputs
        gpio_set_dir(moles[i], GPIO_IN);
        //gpio_pull_up(moles[i]);

        gpio_set_irq_enabled_with_callback(
            moles[i], GPIO_IRQ_EDGE_RISE, true, &gpio_callback
        );
    }

    for(int i = 0; i < 5; i++) {
        gpio_init(lights[i]);
        gpio_set_dir(lights[i], GPIO_OUT);
    }
}






