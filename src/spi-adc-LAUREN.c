// - ADC with potentiometer to change game speed setting
// - SPI with LCD display to show leaderboard stats and update the game score
// - Lauren: SPI + LCD for stats + game score, ADC with potentiometer

#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"

/*
Function initializations:
*/
// For main to call:
void init_disp_spi(); // Initializes display, score, and time. Call at beginning.
void display_score_isr(); // Updates score on display. Call when first start game and when score changes.
void display_time_isr(); // Updates time on display. Call when first start game.
void display_game_over(); // Call if press incorrect or not in time
// Helper functions:
void display_game_over();
void init_display_timer();
void cd_init();
void cd_display1(const char *str);
void cd_display2(const char *str);
void send_spi_cmd(spi_inst_t* spi, uint16_t value);
void send_spi_data(spi_inst_t* spi, uint16_t value);

// Global variables - PUT IN MAIN LATER
const int SPI_DISP_SCK = 26; 
const int SPI_DISP_CSn = 25;
const int SPI_DISP_TX = 27;
int score; // already in
int time_left; // already in

/*
####################
------- ADC -------
####################
*/

adc_init()


/*
####################
------- SPI -------
####################
*/

// Timer 0, alarm 0 used for keeping track of game time
void init_display_timer()
{
    // Enable interrupt for alarm
    hw_set_bits(&timer_hw->inte, 1u << 0);
    // Set handler for alarm
    irq_set_exclusive_handler(TIMER0_IRQ_0, display_time_isr);
    // Enable alarm interrupt
    irq_set_enabled(TIMER0_IRQ_0, true);
    // Set time for alarm
    uint64_t target = timer_hw->timerawl + (10000);
    // Write time to alarm to arm
    timer_hw->alarm[0] = (uint32_t) target;
}

void send_spi_cmd(spi_inst_t* spi, uint16_t value) {
    // Wait until SPI isn't busy
    while(spi_is_busy(spi));

    // Write value
    // Allowed to use SDK or need to write directly to register?
    // spi_write16_blocking(spi, &value, 1);
    spi_get_hw(spi)->dr = value;
}

void send_spi_data(spi_inst_t* spi, uint16_t value) {
    send_spi_cmd(spi, value | 0x100);
}

void cd_init() {
    // Sleep 1ms to let power to display stabilize
    sleep_us(1);

    //Perform function set command: 8-bit interface, 2 line display mode, 11 dots per char
    uint16_t value = 0xF << 2;
    send_spi_cmd(spi1, value);
    // Page 16 of LCD datasheet
    // 0 0 1 DL N F - - = 001111XX = 0xF << 2
    // DL high for 8-bit bus mode
    // N high for 2-line display mode
    // F high for 11 dots display mode

    // Sleep 40us
    sleep_us(40);

    // Perform display on/off command: turn on display, turn off cursor, turn off cursor blink
    value = 0xC;
    send_spi_cmd(spi1, value);
    // Page 15 of LCD datasheet
    // 0 0 0 0 1 D C B = 00001100 = 0xC
    // D high for display  on
    // C low for cursor off
    // B low for cursor blink off

    // Sleep 40us
    sleep_us(40);

    // Perform clear display commmand
    value = 1; 
    send_spi_cmd(spi1, value);
    // Page 15 of LCD datasheet
    // 00000001
    // Clears display data by writing 20H to all DDRAM address, set DDRAM 
    // address to addresscounter, return cursor to orginal space

    // Sleep 2ms
    sleep_ms(2);

    // Perform entry mode set command
    // Move cursor to the right and increment DDRAM address by 1 (don't shift display)
    value = 0x6; 
    send_spi_cmd(spi1, value);
    // Datasheet page 15
    // 0 0 0 0 0 1 I/D SH = 00000110 = 0x6
    // I/D high for cursor moves to right, DDRAM address increased by 1
    // SH low for no entire shift display

    // Sleep 40us
    sleep_us(40);
}

void init_disp_spi()
{
    for(int pin = SPI_7SEG_CSn; pin <= SPI_7SEG_TX; pin++)
    {
        gpio_set_function(pin, GPIO_FUNC_SPI); // GPIO_FUNC_SPI = 1
    }
    spi_init(spi1, 10000);
    spi_set_format(spi1, 9, 0, 0, SPI_MSB_FIRST);

    init_display_timer();
    cd_init();
}

void display_time_isr()
{
    const char *str = "Time left: %d s", time_left;
    cd_display1(str);

    // If time_left = 0, update GameState to GAME_OVER
    if(time_left == 0)
    {
        GameState = GAME_OVER;
        display_game_over();
        return;
    }

    // Acknowledge interrupt for ALARM0 on TIMER1
    timer_hw->intr = 1u << 0;
    uint64_t target = timer_hw->timerawl + (10000);
    timer_hw->alarm[0] = (uint32_t) target;

    time_left = time_left - 1;
}

void display_game_over()
{
    const char *str1 = "Game Over!";
    cd_display1(str1);
    const char *str2 = "Final Score: %d", score;
    cd_display2(str2);
}

void display_score_isr()
{
    const char *str = "Your score: %d", score;
    cd_display1(str)
}

// First line display of LCD
void cd_display1(const char *str) {
    // Send command to move cursor to first line (starts at address 00H)
    uint16_t value = (0x80) | (0x00);
    send_spi_cmd(spi1, value);
    // Datasheet page 16/17
    // DDRAM controls cursor position
    // 1 AC6 AC5 AC4 AC3 AC2 AC1 AC0 = 0x80 | AC
    // AC is DDRAM address

    // Send each character (16) in str to the display using send_spi_data
    for(int let = 0; let < 16; let++)
    {
        send_spi_data(spi1, str[let]);
    }
}

// Second line display of LCD
void cd_display2(const char *str) {
    // Send command to move cursor to second line (starts at address 40H)
    uint16_t value = (0x80) | (0x40); 
    send_spi_cmd(spi1, value);
    // Datasheet page 16/17
    // DDRAM controls cursor position
    // 1 AC6 AC5 AC4 AC3 AC2 AC1 AC0 = 0x80 | AC
    // AC is DDRAM address

    // Send each character (16) in str to the display using send_spi_data
    for(int let = 0; let < 16; let++)
    {
        send_spi_data(spi1, str[let]);
    }
}
