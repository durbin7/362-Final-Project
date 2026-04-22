#include <stdlib.h>

void init_gpio();
void gpio_callback(uint, uint32_t);

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"


static int duty_cycle = 0;
static int dir = 0;
static int color = 0;

void display_init_pins();
void display_init_timer();
void display_char_print(const char message[]);
void keypad_init_pins();
void keypad_init_timer();
void init_wavetable(void);
void set_freq(int chan, float f);
extern KeyEvents kev;
void drum_machine();