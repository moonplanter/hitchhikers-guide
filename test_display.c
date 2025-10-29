/*
 * Using EXACT initialization from working lcdspi.c
 */

#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/gpio.h"

#define LCD_CS     13
#define LCD_SCK    10
#define LCD_MOSI   11
#define LCD_MISO   12
#define LCD_DC     14
#define LCD_RST    15
#define LCD_BL     5

#define LCD_SPI_SPEED 25000000

void spi_write_command(uint8_t cmd) {
    gpio_put(LCD_DC, 0);
    gpio_put(LCD_CS, 0);
    spi_write_blocking(spi1, &cmd, 1);
    gpio_put(LCD_CS, 1);
}

void spi_write_data(uint8_t data) {
    gpio_put(LCD_DC, 1);
    gpio_put(LCD_CS, 0);
    spi_write_blocking(spi1, &data, 1);
    gpio_put(LCD_CS, 1);
}

void reset_controller(void) {
    gpio_put(LCD_RST, 1);
    sleep_ms(10);
    gpio_put(LCD_RST, 0);
    sleep_ms(10);
    gpio_put(LCD_RST, 1);
    sleep_ms(200);
}

void pico_lcd_init(void) {
    reset_controller();

    spi_write_command(0xE0); // Positive Gamma Control
    spi_write_data(0x00);
    spi_write_data(0x03);
    spi_write_data(0x09);
    spi_write_data(0x08);
    spi_write_data(0x16);
    spi_write_data(0x0A);
    spi_write_data(0x3F);
    spi_write_data(0x78);
    spi_write_data(0x4C);
    spi_write_data(0x09);
    spi_write_data(0x0A);
    spi_write_data(0x08);
    spi_write_data(0x16);
    spi_write_data(0x1A);
    spi_write_data(0x0F);

    spi_write_command(0XE1); // Negative Gamma Control
    spi_write_data(0x00);
    spi_write_data(0x16);
    spi_write_data(0x19);
    spi_write_data(0x03);
    spi_write_data(0x0F);
    spi_write_data(0x05);
    spi_write_data(0x32);
    spi_write_data(0x45);
    spi_write_data(0x46);
    spi_write_data(0x04);
    spi_write_data(0x0E);
    spi_write_data(0x0D);
    spi_write_data(0x35);
    spi_write_data(0x37);
    spi_write_data(0x0F);

    spi_write_command(0XC0); // Power Control 1
    spi_write_data(0x17);
    spi_write_data(0x15);

    spi_write_command(0xC1); // Power Control 2
    spi_write_data(0x41);

    spi_write_command(0xC5); // VCOM Control
    spi_write_data(0x00);
    spi_write_data(0x12);
    spi_write_data(0x80);

    spi_write_command(0x36); // Memory Access Control
    spi_write_data(0x48); // MX, BGR

    spi_write_command(0x3A); // Pixel Interface Format
    spi_write_data(0x66); // 18-bit colour

    spi_write_command(0xB0); // Interface Mode Control
    spi_write_data(0x00);

    spi_write_command(0xB1); // Frame Rate Control
    spi_write_data(0xA0);

    spi_write_command(0x21); // Display Inversion ON

    spi_write_command(0xB4); // Display Inversion Control
    spi_write_data(0x02);

    spi_write_command(0xB6); // Display Function Control
    spi_write_data(0x02);
    spi_write_data(0x02);
    spi_write_data(0x3B);

    spi_write_command(0xB7); // Entry Mode Set
    spi_write_data(0xC6);
    
    spi_write_command(0xE9);
    spi_write_data(0x00);

    spi_write_command(0xF7); // Adjust Control 3
    spi_write_data(0xA9);
    spi_write_data(0x51);
    spi_write_data(0x2C);
    spi_write_data(0x82);

    spi_write_command(0x11); // Sleep Out
    sleep_ms(120);

    spi_write_command(0x29); // Display ON
    sleep_ms(120);
}

void fill_screen(uint32_t color) {
    // Set window
    spi_write_command(0x2A);
    spi_write_data(0);
    spi_write_data(0);
    spi_write_data(0x01);
    spi_write_data(0x3F);
    
    spi_write_command(0x2B);
    spi_write_data(0);
    spi_write_data(0);
    spi_write_data(0x01);
    spi_write_data(0x3F);
    
    spi_write_command(0x2C);
    
    // Send RGB data (3 bytes per pixel)
    uint8_t rgb[3];
    rgb[0] = (color >> 16) & 0xFF;
    rgb[1] = (color >> 8) & 0xFF;
    rgb[2] = color & 0xFF;
    
    gpio_put(LCD_DC, 1);
    gpio_put(LCD_CS, 0);
    
    for (uint32_t i = 0; i < 320 * 320; i++) {
        spi_write_blocking(spi1, rgb, 3);
    }
    
    gpio_put(LCD_CS, 1);
}

int main() {
    sleep_ms(200);
    
    // Init GPIO
    gpio_init(LCD_CS);
    gpio_init(LCD_DC);
    gpio_init(LCD_RST);
    gpio_init(LCD_BL);
    
    gpio_set_dir(LCD_CS, GPIO_OUT);
    gpio_set_dir(LCD_DC, GPIO_OUT);
    gpio_set_dir(LCD_RST, GPIO_OUT);
    gpio_set_dir(LCD_BL, GPIO_OUT);
    
    // Init SPI
    spi_init(spi1, LCD_SPI_SPEED);
    gpio_set_function(LCD_SCK, GPIO_FUNC_SPI);
    gpio_set_function(LCD_MOSI, GPIO_FUNC_SPI);
    gpio_set_function(LCD_MISO, GPIO_FUNC_SPI);
    
    gpio_put(LCD_CS, 1);
    gpio_put(LCD_RST, 1);
    gpio_put(LCD_BL, 1);  // Backlight ON
    
    // Initialize display with EXACT sequence
    pico_lcd_init();
    
    // Test colors - RGB888 format
    fill_screen(0xFF0000); // Red
    sleep_ms(1000);
    
    fill_screen(0x00FF00); // Green
    sleep_ms(1000);
    
    fill_screen(0x0000FF); // Blue
    sleep_ms(1000);
    
    fill_screen(0xFFFFFF); // White
    sleep_ms(1000);
    
    // Flash pattern
    for (int i = 0; i < 10; i++) {
        fill_screen(0x00FF00); // Green
        sleep_ms(300);
        fill_screen(0x000000); // Black
        sleep_ms(300);
    }
    
    // Stay green
    fill_screen(0x00FF00);
    
    while (1) {
        tight_loop_contents();
    }
}