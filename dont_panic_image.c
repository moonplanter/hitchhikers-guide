#include "dont_panic_image.h"
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/gpio.h"

extern void spi_write_command(uint8_t cmd);
extern void spi_write_data(uint8_t data);

void draw_compressed_image(const uint32_t* compressed, uint32_t size) {
    // Set full screen window
    spi_write_command(0x2A);
    spi_write_data(0); spi_write_data(0);
    spi_write_data(0x01); spi_write_data(0x3F);
    
    spi_write_command(0x2B);
    spi_write_data(0); spi_write_data(0);
    spi_write_data(0x01); spi_write_data(0x3F);
    
    spi_write_command(0x2C);
    
    // Decompress and send
    gpio_put(14, 1);  // LCD_DC
    gpio_put(13, 0);  // LCD_CS
    
    for (uint32_t i = 0; i < size; i++) {
        uint32_t packed = compressed[i];
        uint8_t count = (packed >> 24) & 0xFF;
        uint8_t r = (packed >> 16) & 0xFF;
        uint8_t g = (packed >> 8) & 0xFF;
        uint8_t b = packed & 0xFF;
        
        uint8_t rgb[3] = {r, g, b};
        for (int j = 0; j < count; j++) {
            spi_write_blocking(spi1, rgb, 3);
        }
    }
    
    gpio_put(13, 1);  // LCD_CS high
}