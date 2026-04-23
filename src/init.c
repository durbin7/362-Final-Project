#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/xosc.h"
#include "pico/multicore.h"
#include "functions.h"

int moles[] = {3,5,7,9,11};
int lights[] = {4,6,8,10,12};
volatile int pressed_mole;
volatile bool hit;

void gpio_callback() { //tell us what button is pressed
    printf("button pressed");
    hit = true;
    for(int i = 0; i < 5; i++)
    {
        if(gpio_get_irq_event_mask(moles[i]) & GPIO_IRQ_EDGE_RISE) //might need to get rid of edge rise
        {
            // Acknowledge event
            gpio_acknowledge_irq(moles[i], GPIO_IRQ_EDGE_RISE);
            pressed_mole = moles[i];
        } 
    }
}

void init_gpio() {
    // 1. Init input pins
    for(int i = 0; i < 5; i++) {
        gpio_init(moles[i]);
        gpio_set_dir(moles[i], GPIO_IN);
        gpio_pull_up(moles[i]);
    }

    // 2. Register handler FIRST
    gpio_set_irq_callback(&gpio_callback);
    irq_set_enabled(IO_IRQ_BANK0, true);

    // 3. Then enable per-pin IRQs
    for (int i = 0; i < 5; i++) {
        gpio_set_irq_enabled(moles[i], GPIO_IRQ_EDGE_RISE, true);
    }

    // 4. Init output pins
    for(int i = 0; i < 5; i++) {
        gpio_init(lights[i]);
        gpio_set_dir(lights[i], GPIO_OUT);
    }
}






