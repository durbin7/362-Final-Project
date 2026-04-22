#include <stdlib.h>
#include <stdbool.h>
#include <functions.h>

int score = 0;
int time_left = 30;
int interval_between_moles = 500; //this is in ms
int highscore = 0;

int active_lit;

extern int moles[];
extern int lights[];
extern int active_mole;
extern bool hit;

typedef enum {
    IDLE,
    PLAYING,
    GAME_OVER
} GameState;

GameState game_status = IDLE;

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

int spawn_mole() {
    int random = rand() % 5;

    active_mole = moles[random];
    active_lit = lights[random];

    gpio_put(active_lit, 1); //turn on

    return active_mole;
}


int main() {
    
    //init_gpio();
    //init_timers();
    //init_pwm();
    //... 
    //enable_gpio_interrupts()
    init_gpio();

    bool start = false;
    game_status = IDLE;

    while (1) {

    
    switch(game_status) {
        case IDLE:
            if (start == true) { //inside will 
                game_status = PLAYING;
                time_left = 30;
                score = 0;

                start = true;

                spawn_mole;
            } 
            break; 

        case PLAYING:
            //spawn_mole()

            if(hit) {
                if (active_mole == active_lit) {
                    score++;
                    //probably call update spi
                }
                else {
                    //call the wrong sound
                }

                gpio_put(active_lit, 0);
            }
            
            if (time_left <= 0) {
                game_status = GAME_OVER;
            }

        case GAME_OVER:
            if (score > highscore) {
                highscore = score;
            }

            gpio_put(active_lit, 0);

            start = false;
            game_status = IDLE;

            break;
        }

    }
}


