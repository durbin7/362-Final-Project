#include <stdio.h>
#include <string.h>

#include "hardware/spi.h"
#include "hardware/gpio.h"
#include "pico/stdlib.h"    

#define SD_SPI_INST spi0
#define SD_MISO_PIN 16 
#define SD_CS_PIN 17 
#define SD_SCK_PIN 18 
#define SD_MOSI_PIN 19 

/*
    Wiring needed: 3v3, GND, 
    all SD card pins


*/

void init_spi_sdcard(void) {
    
    gpio_init(SD_CS_PIN);
    gpio_set_dir(SD_CS_PIN, GPIO_OUT);
    
    gpio_put(SD_CS_PIN, 1);

    gpio_set_function(SD_MISO_PIN, GPIO_FUNC_SPI);
    gpio_set_function(SD_SCK_PIN,  GPIO_FUNC_SPI);
    gpio_set_function(SD_MOSI_PIN, GPIO_FUNC_SPI);

    spi_init(SD_SPI_INST, 400 * 1000);
    spi_set_format(SD_SPI_INST, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);
}    

void disable_sdcard(void) {
    
    gpio_put(SD_CS_PIN, 1);
    uint8_t temp = 0xFF;
    spi_write_blocking(SD_SPI_INST, &temp, 1);

    gpio_init(SD_MOSI_PIN);
    gpio_set_dir(SD_MOSI_PIN, GPIO_OUT);
    gpio_put(SD_MOSI_PIN, 1);
}

void enable_sdcard(void) {
    gpio_set_function(SD_MOSI_PIN, GPIO_FUNC_SPI);

    gpio_put(SD_CS_PIN, 0);
}

void sdcard_io_high_speed(void) {
    spi_set_baudrate(SD_SPI_INST, 12 * 1000 * 1000);
}

void init_sdcard_io(void) {
    init_spi_sdcard();
    disable_sdcard();
}

