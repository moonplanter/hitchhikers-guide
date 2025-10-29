/* Minimal SD Card SPI Driver */
#include "sd_spi.h"
#include "hardware/spi.h"
#include "hardware/gpio.h"
#include "pico/stdlib.h"

#define SD_CS   17
#define SD_SCK  18
#define SD_MOSI 19
#define SD_MISO 16

static inline void sd_cs_select() {
    gpio_put(SD_CS, 0);
    sleep_us(1);
}

static inline void sd_cs_deselect() {
    gpio_put(SD_CS, 1);
    sleep_us(1);
}

void sd_spi_init(void) {
    spi_init(spi0, 400000);  // Start slow
    gpio_set_function(SD_SCK, GPIO_FUNC_SPI);
    gpio_set_function(SD_MOSI, GPIO_FUNC_SPI);
    gpio_set_function(SD_MISO, GPIO_FUNC_SPI);
    
    gpio_init(SD_CS);
    gpio_set_dir(SD_CS, GPIO_OUT);
    gpio_put(SD_CS, 1);
}

uint8_t sd_spi_transfer(uint8_t data) {
    uint8_t rx;
    spi_write_read_blocking(spi0, &data, &rx, 1);
    return rx;
}