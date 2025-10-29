/*
 * Display header for HGTTG PicoCalc
 */

#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdint.h>
#include <stdbool.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/gpio.h"

// Display configuration
#define LCD_WIDTH  320
#define LCD_HEIGHT 320
#define LCD_SPI    spi0
#define LCD_DC     8
#define LCD_CS     9
#define LCD_CLK    10
#define LCD_MOSI   11
#define LCD_RST    12
#define LCD_BL     25

// HGTTG Color scheme (RGB565)
#define COLOR_BLACK    0x0000
#define COLOR_HGTTG    0x07E0  // Bright green
#define COLOR_AMBER    0xFD20  // Amber/orange
#define COLOR_GRAY     0x7BEF  // Light gray
#define COLOR_DARK_BG  0x0841  // Very dark green
#define COLOR_RED      0xF800
#define COLOR_BLUE     0x001F
#define COLOR_WHITE    0xFFFF

// Basic display functions (from main.c)
extern void lcd_write_cmd(uint8_t cmd);
extern void lcd_write_data(uint8_t* data, size_t len);
extern void lcd_set_window(int x0, int y0, int x1, int y1);
extern void lcd_clear(uint16_t color);

// Enhanced display functions
void lcd_draw_pixel(int x, int y, uint16_t color);
void lcd_draw_text(int x, int y, const char* text, uint16_t color);
void lcd_draw_text_bg(int x, int y, const char* text, uint16_t fg, uint16_t bg);
void lcd_fill_rect(int x, int y, int w, int h, uint16_t color);
void lcd_draw_hline(int x, int y, int w, uint16_t color);
void lcd_draw_vline(int x, int y, int h, uint16_t color);
void lcd_draw_rect(int x, int y, int w, int h, uint16_t color);

// Text rendering with word wrap
// Returns total number of lines
int lcd_draw_text_wrapped(int x, int y, int max_width, const char* text, 
                          uint16_t color, int start_line, int max_lines);

// UI Elements
void lcd_draw_progress(int x, int y, int w, int h, int percent, 
                       uint16_t fg, uint16_t bg);
void lcd_draw_title_box(const char* title, uint16_t fg, uint16_t bg);
void lcd_draw_menu_item(int x, int y, const char* text, bool selected, 
                        uint16_t fg, uint16_t sel_fg, uint16_t bg);
void lcd_draw_scrollbar(int total_items, int visible_items, int offset);
void lcd_draw_status_bar(const char* left, const char* right);
void lcd_draw_battery(int x, int y, int percent);

// Animations and effects
void lcd_animate_dont_panic(void);
void lcd_draw_spinner(int x, int y, int frame, uint16_t color);
void lcd_matrix_effect(int duration_ms);

#endif // DISPLAY_H