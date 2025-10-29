/*
 * Enhanced Display Functions for HGTTG PicoCalc
 * Enhanced fonts, colors, and article images
 */

#include "enhanced_display.h"
#include <string.h>
#include <math.h>
#include <stdio.h>

// Additional color definitions (from main file)
#define COLOR_BLACK    0x000000
#define COLOR_GRAY     0x808080

// Forward declarations of LCD functions
extern void lcd_pixel(int x, int y, uint32_t color);
extern void lcd_fill_rect(int x, int y, int w, int h, uint32_t color);
extern void lcd_fill_circle(int xc, int yc, int r, uint32_t color);
extern void lcd_line(int x0, int y0, int x1, int y1, uint32_t color);
extern void lcd_rect(int x, int y, int w, int h, uint32_t color);
extern void lcd_circle(int xc, int yc, int r, uint32_t color);
extern void lcd_text(int x, int y, const char* str, uint32_t color);
extern void lcd_hline(int x0, int x1, int y, uint32_t color);
extern void lcd_vline(int x, int y0, int y1, uint32_t color);

// External font reference
extern const uint8_t font5x7[96][5];

// 12x16 large font (enhanced version of original 5x7)
static const uint16_t large_font_12x16[][16] = {
    // Space (0x20)
    {0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
     0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000},
    // ! (0x21)
    {0x0000, 0x0180, 0x03C0, 0x03C0, 0x03C0, 0x03C0, 0x01C0, 0x0180,
     0x0180, 0x0000, 0x0000, 0x0180, 0x03C0, 0x0180, 0x0000, 0x0000},
    // Add more characters as needed...
    // For now, let's define A-Z and 0-9
    
    // A (0x41)
    {0x0000, 0x0180, 0x03C0, 0x07E0, 0x0660, 0x0C30, 0x0C30, 0x1818,
     0x1FFE, 0x3FFF, 0x300C, 0x7006, 0x6006, 0x0000, 0x0000, 0x0000},
};

void draw_large_text(int x, int y, const char* text, uint32_t color, int scale) {
    int cx = x;
    int cy = y;
    
    while (*text) {
        char c = *text;
        if (c >= 'a' && c <= 'z') c -= 32; // Convert to uppercase
        
        if (c == '\n') {
            cx = x;
            cy += 16 * scale;
        } else if (c >= 32 && c <= 126) {
            // Use original 5x7 font but scaled up
            for (int col = 0; col < 5; col++) {
                uint8_t line = font5x7[c - 32][col];
                for (int row = 0; row < 7; row++) {
                    if (line & (1 << row)) {
                        // Draw scaled pixel block
                        for (int sx = 0; sx < scale; sx++) {
                            for (int sy = 0; sy < scale; sy++) {
                                lcd_pixel(cx + col * scale + sx, cy + row * scale + sy, color);
                            }
                        }
                    }
                }
            }
            cx += 6 * scale;
        } else {
            cx += 6 * scale; // Space for unrecognized chars
        }
        text++;
    }
}

void draw_outlined_text(int x, int y, const char* text, uint32_t fg_color, uint32_t outline_color, int scale) {
    // Draw outline by rendering text at surrounding positions
    for (int ox = -1; ox <= 1; ox++) {
        for (int oy = -1; oy <= 1; oy++) {
            if (ox == 0 && oy == 0) continue;
            draw_large_text(x + ox, y + oy, text, outline_color, scale);
        }
    }
    // Draw main text on top
    draw_large_text(x, y, text, fg_color, scale);
}

void draw_gradient_rect(int x, int y, int w, int h, uint32_t color1, uint32_t color2, bool horizontal) {
    for (int i = 0; i < (horizontal ? w : h); i++) {
        float ratio = (float)i / (horizontal ? w : h);
        
        // Extract RGB components
        uint8_t r1 = (color1 >> 16) & 0xFF;
        uint8_t g1 = (color1 >> 8) & 0xFF;
        uint8_t b1 = color1 & 0xFF;
        
        uint8_t r2 = (color2 >> 16) & 0xFF;
        uint8_t g2 = (color2 >> 8) & 0xFF;
        uint8_t b2 = color2 & 0xFF;
        
        // Interpolate
        uint8_t r = r1 + (uint8_t)((r2 - r1) * ratio);
        uint8_t g = g1 + (uint8_t)((g2 - g1) * ratio);
        uint8_t b = b1 + (uint8_t)((b2 - b1) * ratio);
        
        uint32_t blend_color = (r << 16) | (g << 8) | b;
        
        if (horizontal) {
            lcd_vline(x + i, y, y + h - 1, blend_color);
        } else {
            lcd_hline(x, x + w - 1, y + i, blend_color);
        }
    }
}

void draw_rounded_rect(int x, int y, int w, int h, int radius, uint32_t color) {
    // Draw main rectangle
    lcd_fill_rect(x + radius, y, w - 2 * radius, h, color);
    lcd_fill_rect(x, y + radius, w, h - 2 * radius, color);
    
    // Draw corners using circle segments
    // Top-left
    for (int cy = 0; cy < radius; cy++) {
        for (int cx = 0; cx < radius; cx++) {
            int dx = radius - cx;
            int dy = radius - cy;
            if (dx * dx + dy * dy <= radius * radius) {
                lcd_pixel(x + cx, y + cy, color);
            }
        }
    }
    
    // Top-right
    for (int cy = 0; cy < radius; cy++) {
        for (int cx = 0; cx < radius; cx++) {
            int dx = cx;
            int dy = radius - cy;
            if (dx * dx + dy * dy <= radius * radius) {
                lcd_pixel(x + w - radius + cx, y + cy, color);
            }
        }
    }
    
    // Bottom-left
    for (int cy = 0; cy < radius; cy++) {
        for (int cx = 0; cx < radius; cx++) {
            int dx = radius - cx;
            int dy = cy;
            if (dx * dx + dy * dy <= radius * radius) {
                lcd_pixel(x + cx, y + h - radius + cy, color);
            }
        }
    }
    
    // Bottom-right
    for (int cy = 0; cy < radius; cy++) {
        for (int cx = 0; cx < radius; cx++) {
            int dx = cx;
            int dy = cy;
            if (dx * dx + dy * dy <= radius * radius) {
                lcd_pixel(x + w - radius + cx, y + h - radius + cy, color);
            }
        }
    }
}

void draw_article_header(const char* title, const char* category) {
    // Background gradient
    draw_gradient_rect(0, 0, 320, 50, COLOR_HGTTG_DARK, COLOR_HGTTG_MEDIUM, true);
    
    // Title with outline
    draw_outlined_text(10, 10, title, COLOR_HGTTG_BRIGHT, COLOR_BLACK, 2);
    
    // Category badge
    int cat_len = strlen(category) * 6 + 10;
    draw_rounded_rect(320 - cat_len - 10, 15, cat_len, 20, 10, COLOR_AMBER_MEDIUM);
    lcd_text(320 - cat_len - 5, 20, category, COLOR_BLACK);
}

void draw_babel_fish_diagram(int x, int y) {
    // Main body - yellow fish shape
    draw_gradient_rect(x + 20, y + 30, 80, 25, COLOR_YELLOW_BRIGHT, COLOR_AMBER_BRIGHT, false);
    
    // Head
    lcd_fill_circle(x + 15, y + 42, 8, COLOR_YELLOW_BRIGHT);
    
    // Eye
    lcd_fill_circle(x + 12, y + 40, 3, COLOR_BLACK);
    lcd_pixel(x + 11, y + 39, COLOR_CYAN_BRIGHT);
    
    // Tail fins
    lcd_line(x + 100, y + 25, x + 115, y + 15, COLOR_ORANGE_BRIGHT);
    lcd_line(x + 100, y + 35, x + 115, y + 25, COLOR_ORANGE_BRIGHT);
    lcd_line(x + 100, y + 45, x + 115, y + 35, COLOR_ORANGE_BRIGHT);
    lcd_line(x + 100, y + 55, x + 115, y + 45, COLOR_ORANGE_BRIGHT);
    
    // Brain wave patterns
    for (int i = 0; i < 5; i++) {
        int wave_y = y + 10 + i * 3;
        for (int wx = 0; wx < 40; wx++) {
            int wave_height = (int)(sin((wx + i * 10) * 0.3) * 4);
            lcd_pixel(x + 130 + wx, wave_y + wave_height, COLOR_CYAN_BRIGHT);
        }
    }
    
    // Labels
    lcd_text(x + 130, y + 5, "TELEPATHIC", COLOR_CYAN_MEDIUM);
    lcd_text(x + 130, y + 15, "MATRIX", COLOR_CYAN_MEDIUM);
    
    lcd_text(x + 5, y + 65, "UNIVERSAL", COLOR_AMBER_MEDIUM);
    lcd_text(x + 5, y + 75, "TRANSLATOR", COLOR_AMBER_MEDIUM);
    
    // Direction arrows
    lcd_line(x + 120, y + 42, x + 140, y + 32, COLOR_RED_BRIGHT);
    lcd_line(x + 140, y + 32, x + 135, y + 35, COLOR_RED_BRIGHT);
    lcd_line(x + 140, y + 32, x + 137, y + 37, COLOR_RED_BRIGHT);
}

void draw_earth_diagram(int x, int y) {
    // Planet Earth
    lcd_fill_circle(x + 60, y + 40, 25, COLOR_BLUE_BRIGHT);
    
    // Continents (simplified)
    lcd_fill_circle(x + 55, y + 35, 8, COLOR_HGTTG_MEDIUM);
    lcd_fill_circle(x + 65, y + 45, 6, COLOR_HGTTG_MEDIUM);
    lcd_fill_circle(x + 50, y + 50, 4, COLOR_HGTTG_MEDIUM);
    
    // Explosion/demolition effect
    for (int i = 0; i < 8; i++) {
        float angle = i * 3.14159 / 4;
        int ex = x + 60 + (int)(cos(angle) * 35);
        int ey = y + 40 + (int)(sin(angle) * 35);
        
        lcd_line(x + 60, y + 40, ex, ey, COLOR_RED_BRIGHT);
        lcd_fill_circle(ex, ey, 3, COLOR_ORANGE_BRIGHT);
    }
    
    // "DEMOLISHED" stamp
    draw_rounded_rect(x + 130, y + 20, 80, 25, 5, COLOR_RED_BRIGHT);
    draw_large_text(x + 135, y + 27, "DEMOLISHED", COLOR_YELLOW_BRIGHT, 1);
    
    // Bypass sign
    draw_rounded_rect(x + 20, y + 80, 120, 15, 3, COLOR_YELLOW_BRIGHT);
    lcd_text(x + 25, y + 85, "HYPERSPACE BYPASS", COLOR_BLACK);
    
    // Computer core (hidden purpose)
    lcd_rect(x + 150, y + 50, 60, 40, COLOR_CYAN_MEDIUM);
    lcd_text(x + 155, y + 55, "ORGANIC", COLOR_CYAN_BRIGHT);
    lcd_text(x + 155, y + 65, "COMPUTER", COLOR_CYAN_BRIGHT);
    lcd_text(x + 155, y + 75, "CORE", COLOR_CYAN_BRIGHT);
}

void draw_towel_diagram(int x, int y) {
    // Towel with pattern
    draw_gradient_rect(x + 30, y + 20, 100, 60, COLOR_CYAN_MEDIUM, COLOR_CYAN_BRIGHT, false);
    
    // Towel pattern/texture
    for (int ty = 0; ty < 6; ty++) {
        for (int tx = 0; tx < 10; tx++) {
            if ((tx + ty) % 2 == 0) {
                lcd_fill_rect(x + 30 + tx * 10, y + 20 + ty * 10, 5, 5, COLOR_CYAN_BRIGHT);
            }
        }
    }
    
    // Multiple uses around towel
    // Warmth
    lcd_text(x + 140, y + 15, "WARMTH", COLOR_ORANGE_BRIGHT);
    lcd_line(x + 130, y + 25, x + 115, y + 35, COLOR_ORANGE_BRIGHT);
    
    // Beach
    lcd_text(x + 140, y + 35, "BEACHES", COLOR_YELLOW_BRIGHT);
    lcd_line(x + 130, y + 45, x + 115, y + 50, COLOR_YELLOW_BRIGHT);
    
    // Combat
    lcd_text(x + 140, y + 55, "COMBAT", COLOR_RED_BRIGHT);
    lcd_line(x + 130, y + 65, x + 115, y + 65, COLOR_RED_BRIGHT);
    
    // Emergency signal
    lcd_text(x + 5, y + 90, "EMERGENCY SIGNAL", COLOR_MAGENTA_BRIGHT);
    lcd_line(x + 30, y + 80, x + 50, y + 85, COLOR_MAGENTA_BRIGHT);
    
    // Psychological value indicator
    draw_rounded_rect(x + 150, y + 70, 80, 25, 5, COLOR_PURPLE_MEDIUM);
    lcd_text(x + 155, y + 77, "PSYCHOLOGICAL", COLOR_YELLOW_BRIGHT);
    lcd_text(x + 170, y + 87, "VALUE", COLOR_YELLOW_BRIGHT);
}

void draw_vogon_diagram(int x, int y) {
    // Vogon head (ugly, bureaucratic-looking)
    lcd_fill_rect(x + 40, y + 20, 40, 50, COLOR_HGTTG_DARK);
    
    // Ugly features
    lcd_fill_circle(x + 50, y + 35, 3, COLOR_RED_BRIGHT); // Eye
    lcd_fill_circle(x + 70, y + 35, 3, COLOR_RED_BRIGHT); // Eye
    
    // Large nose
    lcd_fill_rect(x + 58, y + 45, 4, 10, COLOR_HGTTG_MEDIUM);
    
    // Frown
    lcd_line(x + 50, y + 60, x + 70, y + 60, COLOR_RED_BRIGHT);
    
    // Bureaucratic elements
    draw_rounded_rect(x + 90, y + 15, 80, 20, 3, COLOR_AMBER_DARK);
    lcd_text(x + 95, y + 22, "FORM 42B/7", COLOR_YELLOW_BRIGHT);
    
    draw_rounded_rect(x + 90, y + 40, 80, 20, 3, COLOR_AMBER_DARK);
    lcd_text(x + 95, y + 47, "TRIPLICATE", COLOR_YELLOW_BRIGHT);
    
    draw_rounded_rect(x + 90, y + 65, 80, 20, 3, COLOR_AMBER_DARK);
    lcd_text(x + 95, y + 72, "LOST & FOUND", COLOR_YELLOW_BRIGHT);
    
    // Poetry warning
    draw_rounded_rect(x + 180, y + 15, 70, 30, 5, COLOR_RED_BRIGHT);
    lcd_text(x + 185, y + 22, "DANGER!", COLOR_YELLOW_BRIGHT);
    lcd_text(x + 185, y + 32, "VOGON", COLOR_YELLOW_BRIGHT);
    lcd_text(x + 185, y + 42, "POETRY", COLOR_YELLOW_BRIGHT);
    
    // Unpleasantness meter
    lcd_rect(x + 20, y + 80, 100, 15, COLOR_RED_BRIGHT);
    lcd_fill_rect(x + 22, y + 82, 85, 11, COLOR_RED_BRIGHT);
    lcd_text(x + 25, y + 85, "UNPLEASANTNESS: MAX", COLOR_YELLOW_BRIGHT);
}

void draw_heart_of_gold_diagram(int x, int y) {
    // Ship outline (heart shape approximation)
    lcd_fill_circle(x + 50, y + 30, 15, COLOR_YELLOW_BRIGHT);
    lcd_fill_circle(x + 80, y + 30, 15, COLOR_YELLOW_BRIGHT);
    lcd_fill_circle(x + 65, y + 50, 20, COLOR_YELLOW_BRIGHT);
    
    // Improbability drive core
    lcd_fill_circle(x + 65, y + 40, 8, COLOR_CYAN_BRIGHT);
    
    // Probability waves
    for (int i = 0; i < 6; i++) {
        float angle = i * 3.14159 / 3;
        int wave_len = 25 + i * 5;
        int ex = x + 65 + (int)(cos(angle) * wave_len);
        int ey = y + 40 + (int)(sin(angle) * wave_len);
        
        lcd_line(x + 65, y + 40, ex, ey, COLOR_MAGENTA_BRIGHT);
    }
    
    // Drive label
    lcd_text(x + 120, y + 20, "INFINITE", COLOR_CYAN_BRIGHT);
    lcd_text(x + 120, y + 30, "IMPROBABILITY", COLOR_CYAN_BRIGHT);
    lcd_text(x + 120, y + 40, "DRIVE", COLOR_CYAN_BRIGHT);
    
    // Bambleweeny indicator
    draw_rounded_rect(x + 20, y + 80, 120, 15, 3, COLOR_PURPLE_MEDIUM);
    lcd_text(x + 25, y + 85, "BAMBLEWEENY 57 SUB-MESON", COLOR_YELLOW_BRIGHT);
    
    // Improbability factor
    lcd_text(x + 150, y + 60, "FACTOR:", COLOR_AMBER_BRIGHT);
    lcd_text(x + 150, y + 70, "2^276709:1", COLOR_RED_BRIGHT);
    lcd_text(x + 150, y + 80, "AGAINST", COLOR_RED_BRIGHT);
}

void draw_zaphod_diagram(int x, int y) {
    // Two heads
    lcd_fill_circle(x + 40, y + 25, 12, COLOR_AMBER_MEDIUM);
    lcd_fill_circle(x + 70, y + 25, 12, COLOR_AMBER_MEDIUM);
    
    // Eyes
    lcd_fill_circle(x + 38, y + 22, 2, COLOR_BLACK);
    lcd_fill_circle(x + 42, y + 22, 2, COLOR_BLACK);
    lcd_fill_circle(x + 68, y + 22, 2, COLOR_BLACK);
    lcd_fill_circle(x + 72, y + 22, 2, COLOR_BLACK);
    
    // Grins
    lcd_line(x + 35, y + 30, x + 45, y + 30, COLOR_RED_BRIGHT);
    lcd_line(x + 65, y + 30, x + 75, y + 30, COLOR_RED_BRIGHT);
    
    // Body
    lcd_fill_rect(x + 50, y + 40, 20, 30, COLOR_PURPLE_MEDIUM);
    
    // Three arms
    lcd_fill_rect(x + 30, y + 45, 15, 5, COLOR_AMBER_MEDIUM);
    lcd_fill_rect(x + 50, y + 50, 15, 5, COLOR_AMBER_MEDIUM);
    lcd_fill_rect(x + 70, y + 45, 15, 5, COLOR_AMBER_MEDIUM);
    
    // President badge
    draw_rounded_rect(x + 100, y + 20, 70, 20, 5, COLOR_CYAN_BRIGHT);
    lcd_text(x + 105, y + 27, "EX-PRESIDENT", COLOR_BLACK);
    
    // Cool factor
    draw_rounded_rect(x + 100, y + 45, 50, 15, 3, COLOR_ORANGE_BRIGHT);
    lcd_text(x + 105, y + 50, "COOLNESS:", COLOR_BLACK);
    
    // Rating bars
    lcd_fill_rect(x + 100, y + 65, 40, 5, COLOR_YELLOW_BRIGHT);
    lcd_fill_rect(x + 100, y + 72, 35, 5, COLOR_YELLOW_BRIGHT);
    lcd_fill_rect(x + 100, y + 79, 45, 5, COLOR_YELLOW_BRIGHT);
    
    lcd_text(x + 20, y + 85, "HOOPY FROOD", COLOR_CYAN_BRIGHT);
}

void draw_marvin_diagram(int x, int y) {
    // Robot head (large, bulbous)
    lcd_fill_circle(x + 60, y + 30, 18, COLOR_GRAY);
    
    // Glowing red eyes
    lcd_fill_circle(x + 55, y + 25, 3, COLOR_RED_BRIGHT);
    lcd_fill_circle(x + 65, y + 25, 3, COLOR_RED_BRIGHT);
    
    // Frowning mouth
    lcd_line(x + 50, y + 40, x + 70, y + 40, COLOR_RED_BRIGHT);
    lcd_line(x + 50, y + 40, x + 55, y + 35, COLOR_RED_BRIGHT);
    lcd_line(x + 70, y + 40, x + 65, y + 35, COLOR_RED_BRIGHT);
    
    // Body
    lcd_fill_rect(x + 50, y + 50, 20, 25, COLOR_GRAY);
    
    // Depression indicator
    draw_rounded_rect(x + 90, y + 20, 80, 25, 5, COLOR_BLUE_BRIGHT);
    lcd_text(x + 95, y + 27, "DEPRESSION", COLOR_BLACK);
    lcd_text(x + 95, y + 37, "LEVEL: ∞", COLOR_BLACK);
    
    // Brain size indicator
    draw_rounded_rect(x + 90, y + 50, 80, 15, 3, COLOR_PURPLE_MEDIUM);
    lcd_text(x + 95, y + 55, "BRAIN: PLANET-SIZE", COLOR_YELLOW_BRIGHT);
    
    // Pain indicator (left side)
    lcd_text(x + 10, y + 50, "PAIN IN ALL", COLOR_RED_BRIGHT);
    lcd_text(x + 10, y + 60, "DIODES DOWN", COLOR_RED_BRIGHT);
    lcd_text(x + 10, y + 70, "LEFT SIDE", COLOR_RED_BRIGHT);
    
    // Mood quotes
    lcd_text(x + 20, y + 85, "\"Life? Don't talk to me about life.\"", COLOR_CYAN_MEDIUM);
}

void draw_42_diagram(int x, int y) {
    // Large "42" in center
    draw_outlined_text(x + 80, y + 25, "42", COLOR_YELLOW_BRIGHT, COLOR_PURPLE_MEDIUM, 4);
    
    // Deep Thought computer
    draw_rounded_rect(x + 20, y + 15, 40, 30, 5, COLOR_CYAN_MEDIUM);
    lcd_text(x + 25, y + 22, "DEEP", COLOR_BLACK);
    lcd_text(x + 25, y + 32, "THOUGHT", COLOR_BLACK);
    
    // Calculation time
    lcd_text(x + 160, y + 20, "7.5 MILLION", COLOR_AMBER_BRIGHT);
    lcd_text(x + 160, y + 30, "YEARS", COLOR_AMBER_BRIGHT);
    
    // Question marks around the answer
    for (int i = 0; i < 8; i++) {
        float angle = i * 3.14159 / 4;
        int qx = x + 120 + (int)(cos(angle) * 60);
        int qy = y + 50 + (int)(sin(angle) * 30);
        lcd_text(qx, qy, "?", COLOR_RED_BRIGHT);
    }
    
    // Ultimate Question
    draw_rounded_rect(x + 30, y + 75, 150, 20, 5, COLOR_HGTTG_MEDIUM);
    lcd_text(x + 35, y + 82, "ULTIMATE QUESTION: UNKNOWN", COLOR_BLACK);
    
    // Meaning indicator
    draw_rounded_rect(x + 200, y + 50, 60, 30, 8, COLOR_MAGENTA_BRIGHT);
    lcd_text(x + 205, y + 57, "MEANING:", COLOR_BLACK);
    lcd_text(x + 205, y + 67, "UNCLEAR", COLOR_BLACK);
}

void draw_dont_panic_diagram(int x, int y) {
    // Large "DON'T PANIC" text with glow effect
    draw_outlined_text(x + 20, y + 20, "DON'T", COLOR_HGTTG_BRIGHT, COLOR_HGTTG_DARK, 3);
    draw_outlined_text(x + 20, y + 50, "PANIC", COLOR_HGTTG_BRIGHT, COLOR_HGTTG_DARK, 3);
    
    // Guide book representation
    draw_rounded_rect(x + 160, y + 15, 60, 40, 8, COLOR_AMBER_BRIGHT);
    lcd_text(x + 165, y + 22, "GUIDE", COLOR_BLACK);
    lcd_text(x + 165, y + 32, "TO THE", COLOR_BLACK);
    lcd_text(x + 165, y + 42, "GALAXY", COLOR_BLACK);
    
    // Friendly letters indicator
    lcd_text(x + 230, y + 25, "LARGE", COLOR_CYAN_BRIGHT);
    lcd_text(x + 230, y + 35, "FRIENDLY", COLOR_CYAN_BRIGHT);
    lcd_text(x + 230, y + 45, "LETTERS", COLOR_CYAN_BRIGHT);
    
    // Towel reminder
    draw_rounded_rect(x + 40, y + 75, 100, 15, 5, COLOR_CYAN_MEDIUM);
    lcd_text(x + 45, y + 80, "ALWAYS KNOW WHERE YOUR TOWEL IS", COLOR_BLACK);
}

void draw_pan_galactic_diagram(int x, int y) {
    // Glass/mug
    lcd_rect(x + 50, y + 30, 20, 30, COLOR_CYAN_BRIGHT);
    lcd_fill_rect(x + 52, y + 32, 16, 26, COLOR_YELLOW_BRIGHT);
    
    // Bubbles/fizz
    for (int i = 0; i < 8; i++) {
        int bx = x + 55 + (i % 3) * 4;
        int by = y + 35 + i * 3;
        lcd_fill_circle(bx, by, 1, COLOR_CYAN_BRIGHT);
    }
    
    // Lemon slice on rim
    lcd_fill_circle(x + 60, y + 25, 4, COLOR_YELLOW_BRIGHT);
    lcd_line(x + 58, y + 25, x + 62, y + 25, COLOR_ORANGE_BRIGHT);
    
    // Gold brick representation
    draw_rounded_rect(x + 80, y + 40, 30, 15, 3, COLOR_YELLOW_BRIGHT);
    lcd_text(x + 85, y + 45, "GOLD", COLOR_BLACK);
    lcd_text(x + 85, y + 53, "BRICK", COLOR_BLACK);
    
    // Impact effect
    for (int i = 0; i < 6; i++) {
        float angle = i * 3.14159 / 3;
        int ex = x + 95 + (int)(cos(angle) * 25);
        int ey = y + 47 + (int)(sin(angle) * 15);
        lcd_line(x + 95, y + 47, ex, ey, COLOR_RED_BRIGHT);
    }
    
    // Danger warning
    draw_rounded_rect(x + 120, y + 20, 80, 40, 5, COLOR_RED_BRIGHT);
    lcd_text(x + 125, y + 27, "DANGER!", COLOR_YELLOW_BRIGHT);
    lcd_text(x + 125, y + 37, "LIKE HAVING", COLOR_YELLOW_BRIGHT);
    lcd_text(x + 125, y + 47, "BRAINS SMASHED", COLOR_YELLOW_BRIGHT);
    
    // Ingredients list
    lcd_text(x + 20, y + 70, "INGREDIENTS: OL' JANX SPIRIT +", COLOR_AMBER_MEDIUM);
    lcd_text(x + 20, y + 80, "SANTRAGINUS V WATER + MORE...", COLOR_AMBER_MEDIUM);
}

void draw_menu_item(int x, int y, const char* text, int number, bool selected) {
    uint32_t bg_color = selected ? COLOR_HGTTG_MEDIUM : COLOR_HGTTG_DARK;
    uint32_t fg_color = selected ? COLOR_BLACK : COLOR_HGTTG_BRIGHT;
    
    draw_rounded_rect(x, y, 200, 25, 8, bg_color);
    
    char num_str[8];
    snprintf(num_str, sizeof(num_str), "%d.", number);
    
    lcd_text(x + 10, y + 8, num_str, fg_color);
    lcd_text(x + 25, y + 8, text, fg_color);
}

void draw_scroll_indicator(int x, int y, int total_lines, int visible_lines, int scroll_pos) {
    if (total_lines <= visible_lines) return;
    
    int bar_height = 100;
    int thumb_height = (visible_lines * bar_height) / total_lines;
    int thumb_pos = (scroll_pos * (bar_height - thumb_height)) / (total_lines - visible_lines);
    
    // Track
    lcd_rect(x, y, 8, bar_height, COLOR_GRAY);
    
    // Thumb
    lcd_fill_rect(x + 1, y + thumb_pos, 6, thumb_height, COLOR_HGTTG_BRIGHT);
}

void draw_loading_animation(int x, int y, int frame) {
    const char* spinner = "|/-\\";
    char spinner_char = spinner[frame % 4];
    
    draw_large_text(x, y, &spinner_char, COLOR_HGTTG_BRIGHT, 2);
    
    // Surrounding dots
    for (int i = 0; i < 8; i++) {
        float angle = i * 3.14159 / 4;
        int dx = x + 20 + (int)(cos(angle) * 15);
        int dy = y + 10 + (int)(sin(angle) * 15);
        
        uint32_t dot_color = (i == (frame % 8)) ? COLOR_HGTTG_BRIGHT : COLOR_HGTTG_DARK;
        lcd_fill_circle(dx, dy, 2, dot_color);
    }
}
