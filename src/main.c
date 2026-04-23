#include <stdlib.h>
#include <stdbool.h>
#include <functions.h>

extern int score;
extern int time_left;
extern int highscore;
int interval_between_moles = 500; //this is in ms
int indicy_of_mole;
int active_lit;

extern int moles[];
extern int lights[];
int active_mole;
extern bool hit;
extern int pressed_mole;

extern int SPI_DISP_SCK; 
extern int SPI_DISP_CSn;
extern int SPI_DISP_TX;
extern int ADC_CH5; 

typedef enum {
    IDLE,
    PLAYING,
    GAME_OVER
} GameState;

typedef enum {
    SLOW,
    MEDIUM,
    FAST
} GameSpeed;

extern GameSpeed game_speed;
extern GameState game_status;

/***
 * IGNORE NOTES
 * CONFIGURE
 * timer 0 
 * -> should be configured to go off every second
 * -> time_left--
 * -> update_spi
 * GPIO_PINS
 * -> configure them to raise interrupts
 * -> once a player hits it, we check to see if it's the same as the mole
 * 
 *  spawn_mole()
 *  1. turn on an LED / output pin
 *  2. maybe sets another timer for interval time
 *  3. 
 * 
 *  acknowledge_mole_failed(gpio_pin)
 *  1. if time expires
 *  2. turn mole off
 *  3. don't update score
 *  4. play missed sound
 *  5. 
 * 
 *  acknowledge_mole (gpio_pin)
 *  1. this function should acknowledge the interrupt on mole and see if it's the right one
 *  2. play sound (pwm)
 *  2. update the score and call update_spi
 *  3. if time_left, turn off mole and spawn another
 *  
 * 
 *  update_spi() 
 *  1. this should update the score_board
 *  2. Maybe should display stats if time is done
 *  
 *  
 * 
 *  
 * 
 */

void spawn_mole() {
    srand(time(NULL));
    int random = rand() % 5;

    active_mole = moles[random];
    active_lit = lights[random];
    indicy_of_mole = random;

    gpio_put(active_lit, 1); //turn on
    printf("mole spawned: at %d\n", lights[random]);
    //return active_mole;
}



int main() {
    printf("In main()\n");
    //init_gpio();
    //init_timers();
    //init_pwm();
    //... 
    //enable_gpio_interrupts()
    init_gpio();
    init_adc();
    init_display_timer();
    init_disp_spi();

    bool start = true; //should make this a buttonb                   b                                                                                                                       bbv 
    game_status = IDLE;

    for (int i = 4; i <= 12; i+=2) {
        gpio_put(i, 0);
    }

    //display_welcome(); //will just have to call here since no idle state
    while (1) {
    //spawn_mole();
    
    switch(game_status) {
        case IDLE:
            if (start == true) { //inside will 
                game_status = PLAYING;
                time_left = 30;
                score = 0;

                start = true;
                read_adc();
                spawn_mole();
                //display_time_isr();
            } 
          //  display_welcome(); 
            break; 

        case PLAYING:
            

            if(hit) {
                if (active_mole == pressed_mole) {
                    score++;
                    //probably call update spi
                    display_score_isr();
                }
                else {
                    //call the wrong sound
                }

                gpio_put(active_lit, 0);
                spawn_mole(); //reset the cycle
                hit = false;
            }
            
            if (time_left <= 0) {
                game_status = GAME_OVER;
            }
            break;
        case GAME_OVER:
            if (score > highscore) {
                highscore = score;
            }

            gpio_put(active_lit, 0);

            start = false;
            game_status = IDLE;
            display_game_over();

            break;
        }

    }
}


