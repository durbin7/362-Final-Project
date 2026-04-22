//good luck on teh
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "queue.h"
#include "support.h"

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

//Ab is bad  415.3 Hz
//B is good 494 hz
uint32_t step0 = (494 * 1024 / 22050) << 16; //b
uint32_t step1 = (415 * 1024/ 22050) << 16;  //ab or aflat/gfla


void init_pwm_static () //implementing static duty cycle pwm signal
{

    uint32_t period = 0;
    uint32_t duty_cycle = 0;

   //config 37 38 39 as pwm outputs. diff than gpio
    gpio_set_function(37, GPIO_FUNC_PWM); //10b
    gpio_set_function(38, GPIO_FUNC_PWM);//11a
    gpio_set_function(39, GPIO_FUNC_PWM);//11b

    //find out which pwm slcie is connected to gpio
    // uint slice_num = pwm_gpio_to_slice_num(37);
    uint slice_num = pwm_gpio_to_slice_num(37);
    uint slice_num1 = pwm_gpio_to_slice_num(38);
    uint slice_num2 = pwm_gpio_to_slice_num(39);
    //set pwm slice clk divider value to be 150. divides down so CLK DIVIDER IS 1MHZ?
    pwm_config config = pwm_get_default_config(); //1083
    pwm_config_set_clkdiv_int_frac4(&config,150,0 ); //set to 150.274 rasp pi
    pwm_set_clkdiv(slice_num, 150.f);
    pwm_set_clkdiv(slice_num1, 150.f);
    pwm_set_clkdiv(slice_num2, 150.f);
    // clkfreq to 1MHZ for pwn only  
   
    // pwm_init(slice_num, &config, false); //intialize 1083
    // pwm_init(slice_num1, &config, false); //intialize 1083
    // pwm_init()
    //set wrapping counter val of pin to val passed in period - 1
    //max val is period -1
   
    pwm_hw->slice[slice_num].top = period -1;
       pwm_hw->slice[slice_num1].top = period -1;
       pwm_hw->slice[slice_num2].top = period -1;
    //^^ found by diving into pwm_set_wrap

    //for each pin, set corresponding pwm channel duty cycle to val
    //passed in duty cycle
   // uint level = duty_cycle * period;
     pwm_set_chan_level(slice_num, PWM_CHAN_B,duty_cycle );
    pwm_set_chan_level(slice_num1, PWM_CHAN_A,duty_cycle );
    pwm_set_chan_level(slice_num2,PWM_CHAN_B, duty_cycle );

    //enable pwn signals for all pins
    pwm_set_enabled(slice_num, true);
    pwm_set_enabled(slice_num1, true);
    pwm_set_enabled(slice_num2, true);
}
void play_sound ()
{
    //find the wrap for the frequency we want
    //f = 150mhz / (wrap+1) so wrap = (150mhz/freq)
    //default clk is 125mhz
    uint32_t clk = 150000000;
    uint32_t wrap = (clk / freq) -1;
    //writing a specific value to the appropriate PWM interrupt register
    uint slice_num = pwm_gpio_to_slice_num(37);

    pwm_clear_irq(slice_num);//acknowledge

}



void pwm_audio_handler() {
    // fill in
      // fill in
     uint slice_num0 = pwm_gpio_to_slice_num(36); //pin 36 is the pwm output
     pwm_clear_irq(slice_num0); //ack
     offset0 +=step0;
     offset1 += step1;
    
     
    //  int samp = wavetable[offset0 >> 16] + wavetable[ offset1 >> 16];
     
     int samp = (wavetable[(offset0 >> 16) % 1024] + wavetable[ (offset1 >> 16) % 1024]) /2;
    uint32_t unsignedsamp = samp +32768;
    uint32_t top = pwm_hw->slice[slice_num0].top;

    uint32_t level;
    level = (unsignedsamp * top) / 65536;
    pwm_set_chan_level(slice_num0,0,samp);

}


void init_pwm_audio() {

  //config as out 36
     gpio_set_function(36, GPIO_FUNC_PWM); //set pin 36 to pwm output
     uint slice_num0 = pwm_gpio_to_slice_num(36); 
    pwm_set_clkdiv(slice_num0, 150.f); //set pwm slice clk divider to be 150 so 1MHz clock

    pwm_set_chan_level(slice_num0,0,0);
    pwm_set_wrap(slice_num0,1000000/RATE -1); //1079 //results in 20khz frequency
    duty_cycle =0;
    init_wavetable();
    pwm_clear_irq(slice_num0);


    irq_set_enabled(PWM_IRQ_WRAP_0, true);
    irq_set_exclusive_handler(PWM_IRQ_WRAP_0,pwm_audio_handler);

    pwm_set_irq0_enabled(slice_num0, true);

    //start pwm
    pwm_set_enabled(slice_num0, true); //enable slice
  
}
//in main, call init_pwm_audio and call 'set_freq' with float value ___ to get freq of ___
//if button pushed on time, set freq to __, if button pushed when led not on set freq to __