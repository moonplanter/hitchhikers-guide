/*
 * Display functions for HGTTG PicoCalc
 * Includes font rendering, text wrapping, and UI helpers
 */

#include "display.h"
#include "font.h"
#include <string.h>

// Draw a single pixel
void lcd_draw_pixel(int x, int y, uint16_t color) {
    if (x < 0 || x >= LCD_WIDTH || y < 0 || y >= LCD_HEIGHT) return;
    
    lcd_set_window(x, y, x, y);
    
    uint8_t data[2];
    data[0] = color >> 8;
    data[1] = color & 0xFF;
    
    gpio_put(LCD_CS, 0);
    gpio_put(LCD_DC, 1);
    spi_write_blocking(LCD_SPI, data, 2);
    gpio_put(LCD_CS, 1);
}

// Draw text using bitmap font
void lcd_draw_text(int x, int y, const char* text, uint16_t color) {
    draw_string_8x8(x, y, text, color, lcd_draw_pixel);
}

// Draw text with background
void lcd_draw_text_bg(int x, int y, const char* text, uint16_t fg, uint16_t bg) {
    int len = strlen(text);
    
    // Draw background rectangle
    lcd_fill_rect(x - 1, y - 1, len * 8 + 2, 10, bg);
    
    // Draw text on top
    lcd_draw_text(x, y, text, fg);
}

// Fill a rectangle
void lcd_fill_rect(int x, int y, int w, int h, uint16_t color) {
    if (x < 0) { w += x; x = 0; }
    if (y < 0) { h += y; y = 0; }
    if (x + w > LCD_WIDTH) w = LCD_WIDTH - x;
    if (y + h > LCD_HEIGHT) h = LCD_HEIGHT - y;
    if (w <= 0 || h <= 0) return;
    
    lcd_set_window(x, y, x + w - 1, y + h - 1);
    
    uint8_t data[2];
    data[0] = color >> 8;
    data[1] = color & 0xFF;
    
    gpio_put(LCD_CS, 0);
    gpio_put(LCD_DC, 1);
    
    for (int i = 0; i < w * h; i++) {
        spi_write_blocking(LCD_SPI, data, 2);
    }
    
    gpio_put(LCD_CS, 1);
}

// Draw a horizontal line
void lcd_draw_hline(int x, int y, int w, uint16_t color) {
    lcd_fill_rect(x, y, w, 1, color);
}

// Draw a vertical line
void lcd_draw_vline(int x, int y, int h, uint16_t color) {
    lcd_fill_rect(x, y, 1, h, color);
}

// Draw a rectangle outline
void lcd_draw_rect(int x, int y, int w, int h, uint16_t color) {
    lcd_draw_hline(x, y, w, color);
    lcd_draw_hline(x, y + h - 1, w, color);
    lcd_draw_vline(x, y, h, color);
    lcd_draw_vline(x + w - 1, y, h, color);
}

// Word-wrapped text rendering
int lcd_draw_text_wrapped(int x, int y, int max_width, const char* text, 
                          uint16_t color, int start_line, int max_lines) {
    int cursor_x = x;
    int cursor_y = y;
    int line_count = 0;
    int displayed_lines = 0;
    int word_start = 0;
    int i = 0;
    
    char word[64];
    int word_len = 0;
    
    while (text[i] != '\0' && displayed_lines < max_lines) {
        char c = text[i];
        
        // Handle newlines
        if (c == '\n') {
            if (word_len > 0 && line_count >= start_line) {
                word[word_len] = '\0';
                lcd_draw_text(cursor_x, cursor_y, word, color);
                word_len = 0;
            }
            cursor_x = x;
            cursor_y += 10;
            line_count++;
            if (line_count >= start_line) {
                displayed_lines++;
            }
            i++;
            continue;
        }
        
        // Build words
        if (c == ' ' || c == '\t') {
            if (word_len > 0) {
                word[word_len] = '\0';
                int word_width = word_len * 8;
                
                // Check if word fits on current line
                if (cursor_x + word_width > x + max_width) {
                    cursor_x = x;
                    cursor_y += 10;
                    line_count++;
                    if (line_count > start_line && displayed_lines < max_lines) {
                        displayed_lines++;
                    }
                }
                
                // Draw word if in visible range
                if (line_count >= start_line && displayed_lines < max_lines) {
                    lcd_draw_text(cursor_x, cursor_y, word, color);
                }
                
                cursor_x += word_width + 8; // Add space width
                word_len = 0;
            } else {
                cursor_x += 8; // Just a space
            }
        } else {
            word[word_len++] = c;
            if (word_len >= 63) {
                // Word too long, force break
                word[word_len] = '\0';
                if (line_count >= start_line && displayed_lines < max_lines) {
                    lcd_draw_text(cursor_x, cursor_y, word, color);
                }
                cursor_x = x;
                cursor_y += 10;
                line_count++;
                if (line_count > start_line) {
                    displayed_lines++;
                }
                word_len = 0;
            }
        }
        
        i++;
    }
    
    // Draw remaining word
    if (word_len > 0 && line_count >= start_line && displayed_lines < max_lines) {
        word[word_len] = '\0';
        lcd_draw_text(cursor_x, cursor_y, word, color);
    }
    
    return line_count + 1; // Return total lines
}

// Draw a progress bar
void lcd_draw_progress(int x, int y, int w, int h, int percent, 
                       uint16_t fg, uint16_t bg) {
    lcd_draw_rect(x, y, w, h, fg);
    
    int fill_w = ((w - 4) * percent) / 100;
    if (fill_w > 0) {
        lcd_fill_rect(x + 2, y + 2, fill_w, h - 4, fg);
    }
    
    // Fill remainder with background
    if (fill_w < w - 4) {
        lcd_fill_rect(x + 2 + fill_w, y + 2, w - 4 - fill_w, h - 4, bg);
    }
}

// Draw a fancy title box (HGTTG style)
void lcd_draw_title_box(const char* title, uint16_t fg, uint16_t bg) {
    lcd_fill_rect(0, 0, LCD_WIDTH, 40, bg);
    lcd_draw_rect(5, 5, LCD_WIDTH - 10, 30, fg);
    
    int title_len = strlen(title);
    int title_x = (LCD_WIDTH - (title_len * 8)) / 2;
    lcd_draw_text(title_x, 13, title, fg);
}

// Draw a menu item
void lcd_draw_menu_item(int x, int y, const char* text, bool selected, 
                        uint16_t fg, uint16_t sel_fg, uint16_t bg) {
    if (selected) {
        lcd_fill_rect(x - 2, y - 1, LCD_WIDTH - x, 10, bg);
        char buffer[80];
        snprintf(buffer, sizeof(buffer), "> %s", text);
        lcd_draw_text(x, y, buffer, sel_fg);
    } else {
        lcd_draw_text(x + 10, y, text, fg);
    }
}

// Scrollbar indicator
void lcd_draw_scrollbar(int total_items, int visible_items, int offset) {
    if (total_items <= visible_items) return;
    
    int bar_x = LCD_WIDTH - 8;
    int bar_y = 50;
    int bar_h = LCD_HEIGHT - 70;
    
    // Draw track
    lcd_fill_rect(bar_x, bar_y, 4, bar_h, COLOR_GRAY);
    
    // Calculate thumb size and position
    int thumb_h = (bar_h * visible_items) / total_items;
    if (thumb_h < 10) thumb_h = 10;
    
    int thumb_y = bar_y + ((bar_h - thumb_h) * offset) / (total_items - visible_items);
    
    // Draw thumb
    lcd_fill_rect(bar_x, thumb_y, 4, thumb_h, COLOR_HGTTG);
}

// Animate "DON'T PANIC" with pulsing effect
void lcd_animate_dont_panic(void) {
    static int phase = 0;
    const int steps = 20;
    
    for (int i = 0; i < steps; i++) {
        float scale = 0.8f + 0.2f * sinf((phase + i) * 3.14159f / steps);
        uint16_t color = (uint16_t)(0x07E0 * scale); // Dim the green
        
        lcd_clear(COLOR_BLACK);
        
        int y = LCD_HEIGHT / 2 - 40;
        lcd_draw_text(40, y, "DON'T", color);
        lcd_draw_text(40, y + 30, "PANIC", color);
        
        sleep_ms(50);
    }
    
    phase++;
}

// Draw a loading spinner
void lcd_draw_spinner(int x, int y, int frame, uint16_t color) {
    const char spinner[] = {'-', '\\', '|', '/'};
    char buf[2] = {spinner[frame % 4], '\0'};
    lcd_draw_text(x, y, buf, color);
}

// Easter egg: Matrix-style falling characters
void lcd_matrix_effect(int duration_ms) {
    const int cols = LCD_WIDTH / 8;
    int drops[cols];
    
    // Initialize drops
    for (int i = 0; i < cols; i++) {
        drops[i] = rand() % LCD_HEIGHT;
    }
    
    absolute_time_t start = get_absolute_time();
    
    while (absolute_time_diff_us(start, get_absolute_time()) < duration_ms * 1000) {
        // Fade effect
        for (int i = 0; i < LCD_WIDTH * LCD_HEIGHT; i++) {
            // This would need a framebuffer implementation
            // Simplified version: just redraw
        }
        
        for (int i = 0; i < cols; i++) {
            char c = 33 + (rand() % 94); // Random printable char
            lcd_draw_text(i * 8, drops[i], &c, COLOR_HGTTG);
            
            drops[i] += 10;
            if (drops[i] > LCD_HEIGHT && rand() % 10 > 7) {
                drops[i] = 0;
            }
        }
        
        sleep_ms(50);
    }
}

// Draw status bar at bottom
void lcd_draw_status_bar(const char* left, const char* right) {
    int y = LCD_HEIGHT - 12;
    lcd_fill_rect(0, y, LCD_WIDTH, 12, COLOR_DARK_BG);
    
    if (left) {
        lcd_draw_text(5, y + 2, left, COLOR_GRAY);
    }
    
    if (right) {
        int len = strlen(right);
        lcd_draw_text(LCD_WIDTH - (len * 8) - 5, y + 2, right, COLOR_GRAY);
    }
}

// Draw battery indicator
void lcd_draw_battery(int x, int y, int percent) {
    uint16_t color;
    if (percent > 50) color = COLOR_HGTTG;
    else if (percent > 20) color = COLOR_AMBER;
    else color = 0xF800; // Red
    
    lcd_draw_rect(x, y, 20, 10, COLOR_GRAY);
    lcd_fill_rect(x + 20, y + 3, 2, 4, COLOR_GRAY);
    
    int fill = (16 * percent) / 100;
    if (fill > 0) {
        lcd_fill_rect(x + 2, y + 2, fill, 6, color);
    }
}