// - ADC with potentiometer to change game speed setting
// - SPI with LCD display to show leaderboard stats and update the game score
// - Lauren: SPI + LCD for stats + game score, ADC with potentiometer

#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/gpio.h"

// Function initializations:


// Global variables - PUT IN MAIN LATER
const int SPI_DISP_SCK = 26; // Replace with your SCK pin number for the LCD/OLED display
const int SPI_DISP_CSn = 25; // Replace with your CSn pin number for the LCD/OLED display
const int SPI_DISP_TX = 27; // Replace with your TX pin number for the LCD/OLED display

/*
####################
------- ADC -------
####################
*/




/*
####################
------- SPI -------
####################
*/




