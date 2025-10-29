#include "pico/stdlib.h"
#define LCD_BL 5

int main() {
    gpio_init(LCD_BL);
    gpio_set_dir(LCD_BL, GPIO_OUT);
    
    while (1) {
        gpio_put(LCD_BL, 1);
        sleep_ms(1000);
        gpio_put(LCD_BL, 0);
        sleep_ms(1000);
    }
}
