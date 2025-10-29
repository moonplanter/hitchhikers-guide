#ifndef ENHANCED_DISPLAY_H
#define ENHANCED_DISPLAY_H

#include <stdint.h>
#include <stdbool.h>

// Enhanced color palette for better aesthetics
#define COLOR_HGTTG_BRIGHT    0x00FF00  // Bright green
#define COLOR_HGTTG_MEDIUM    0x00CC00  // Medium green  
#define COLOR_HGTTG_DARK      0x008800  // Dark green
#define COLOR_AMBER_BRIGHT    0xFFB000  // Bright amber
#define COLOR_AMBER_MEDIUM    0xCC8800  // Medium amber
#define COLOR_AMBER_DARK      0x996600  // Dark amber
#define COLOR_CYAN_BRIGHT     0x00FFFF  // Bright cyan
#define COLOR_CYAN_MEDIUM     0x00CCCC  // Medium cyan
#define COLOR_MAGENTA_BRIGHT  0xFF00FF  // Bright magenta
#define COLOR_YELLOW_BRIGHT   0xFFFF00  // Bright yellow
#define COLOR_RED_BRIGHT      0xFF4040  // Bright red
#define COLOR_BLUE_BRIGHT     0x4080FF  // Bright blue
#define COLOR_PURPLE_MEDIUM   0x8040FF  // Medium purple
#define COLOR_ORANGE_BRIGHT   0xFF8000  // Bright orange

// Enhanced font rendering functions
void draw_large_text(int x, int y, const char* text, uint32_t color, int scale);
void draw_outlined_text(int x, int y, const char* text, uint32_t fg_color, uint32_t outline_color, int scale);
void draw_gradient_rect(int x, int y, int w, int h, uint32_t color1, uint32_t color2, bool horizontal);
void draw_rounded_rect(int x, int y, int w, int h, int radius, uint32_t color);
void draw_article_header(const char* title, const char* category);

// Article image functions
void draw_babel_fish_diagram(int x, int y);
void draw_earth_diagram(int x, int y);
void draw_towel_diagram(int x, int y);
void draw_vogon_diagram(int x, int y);
void draw_heart_of_gold_diagram(int x, int y);
void draw_zaphod_diagram(int x, int y);
void draw_marvin_diagram(int x, int y);
void draw_42_diagram(int x, int y);
void draw_dont_panic_diagram(int x, int y);
void draw_pan_galactic_diagram(int x, int y);

// UI enhancement functions
void draw_menu_item(int x, int y, const char* text, int number, bool selected);
void draw_scroll_indicator(int x, int y, int total_lines, int visible_lines, int scroll_pos);
void draw_loading_animation(int x, int y, int frame);

#endif
