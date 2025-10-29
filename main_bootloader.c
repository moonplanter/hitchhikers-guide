/*
 * Hitchhiker's Guide to the Galaxy - Simple PicoCalc Version
 * Compatible with PicoCalc bootloader
 * No SD card required - articles stored in flash
 * 
 * CORRECTED for ILI9488 display with proper initialization
 * v2.0 - Added Search functionality
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/i2c.h"
#include "hardware/gpio.h"

#include "dont_panic_image.h"
#include "enhanced_display.h"


// Display pins - CORRECTED for PicoCalc
#define LCD_WIDTH  320
#define LCD_HEIGHT 320
#define LCD_CS     13
#define LCD_SCK    10
#define LCD_MOSI   11
#define LCD_MISO   12
#define LCD_DC     14
#define LCD_RST    15
#define LCD_BL     5
#define LCD_SPI_SPEED 25000000

// Keyboard I2C - CORRECTED from i2ckbd.h
#define KBD_I2C    i2c1       // i2c1, not i2c0!
#define KBD_SDA    6
#define KBD_SCL    7
#define KBD_ADDR   0x1F       // 0x1F, not 0x55!
#define KBD_SPEED  10000      // 10kHz!

// Colors (RGB888 format for ILI9488)
#define COLOR_BLACK    0x000000
#define COLOR_HGTTG    0x00FF00  // Bright green
#define COLOR_AMBER    0xFF8000  // Amber/orange
#define COLOR_GRAY     0x808080
#define COLOR_DARK     0x001000  // Very dark green
#define COLOR_BLUE     0x3076C0  // Titles & main text
#define COLOR_GREEN    0x5BC065  // Diagram labels
#define COLOR_DARKGRN  0x345730  // Darker UI accents
#define COLOR_LIGHTRED 0xC0655B  // Secondary highlight
#define COLOR_DARKRED  0x573034
#define COLOR_YELLOW   0xC0B05B
#define COLOR_LILAC    0xB05BC0


// Simple font (5x7)
const uint8_t font5x7[96][5] = {
    {0x00, 0x00, 0x00, 0x00, 0x00}, // Space
    {0x00, 0x00, 0x5F, 0x00, 0x00}, // !
    {0x00, 0x07, 0x00, 0x07, 0x00}, // "
    {0x14, 0x7F, 0x14, 0x7F, 0x14}, // #
    {0x24, 0x2A, 0x7F, 0x2A, 0x12}, // $
    {0x23, 0x13, 0x08, 0x64, 0x62}, // %
    {0x36, 0x49, 0x55, 0x22, 0x50}, // &
    {0x00, 0x05, 0x03, 0x00, 0x00}, // '
    {0x00, 0x1C, 0x22, 0x41, 0x00}, // (
    {0x00, 0x41, 0x22, 0x1C, 0x00}, // )
    {0x08, 0x2A, 0x1C, 0x2A, 0x08}, // *
    {0x08, 0x08, 0x3E, 0x08, 0x08}, // +
    {0x00, 0x50, 0x30, 0x00, 0x00}, // ,
    {0x08, 0x08, 0x08, 0x08, 0x08}, // -
    {0x00, 0x60, 0x60, 0x00, 0x00}, // .
    {0x20, 0x10, 0x08, 0x04, 0x02}, // /
    {0x3E, 0x51, 0x49, 0x45, 0x3E}, // 0
    {0x00, 0x42, 0x7F, 0x40, 0x00}, // 1
    {0x42, 0x61, 0x51, 0x49, 0x46}, // 2
    {0x21, 0x41, 0x45, 0x4B, 0x31}, // 3
    {0x18, 0x14, 0x12, 0x7F, 0x10}, // 4
    {0x27, 0x45, 0x45, 0x45, 0x39}, // 5
    {0x3C, 0x4A, 0x49, 0x49, 0x30}, // 6
    {0x01, 0x71, 0x09, 0x05, 0x03}, // 7
    {0x36, 0x49, 0x49, 0x49, 0x36}, // 8
    {0x06, 0x49, 0x49, 0x29, 0x1E}, // 9
    {0x00, 0x36, 0x36, 0x00, 0x00}, // :
    {0x00, 0x56, 0x36, 0x00, 0x00}, // ;
    {0x00, 0x08, 0x14, 0x22, 0x41}, // <
    {0x14, 0x14, 0x14, 0x14, 0x14}, // =
    {0x41, 0x22, 0x14, 0x08, 0x00}, // >
    {0x02, 0x01, 0x51, 0x09, 0x06}, // ?
    {0x32, 0x49, 0x79, 0x41, 0x3E}, // @
    {0x7E, 0x11, 0x11, 0x11, 0x7E}, // A
    {0x7F, 0x49, 0x49, 0x49, 0x36}, // B
    {0x3E, 0x41, 0x41, 0x41, 0x22}, // C
    {0x7F, 0x41, 0x41, 0x22, 0x1C}, // D
    {0x7F, 0x49, 0x49, 0x49, 0x41}, // E
    {0x7F, 0x09, 0x09, 0x01, 0x01}, // F
    {0x3E, 0x41, 0x41, 0x51, 0x32}, // G
    {0x7F, 0x08, 0x08, 0x08, 0x7F}, // H
    {0x00, 0x41, 0x7F, 0x41, 0x00}, // I
    {0x20, 0x40, 0x41, 0x3F, 0x01}, // J
    {0x7F, 0x08, 0x14, 0x22, 0x41}, // K
    {0x7F, 0x40, 0x40, 0x40, 0x40}, // L
    {0x7F, 0x02, 0x04, 0x02, 0x7F}, // M
    {0x7F, 0x04, 0x08, 0x10, 0x7F}, // N
    {0x3E, 0x41, 0x41, 0x41, 0x3E}, // O
    {0x7F, 0x09, 0x09, 0x09, 0x06}, // P
    {0x3E, 0x41, 0x51, 0x21, 0x5E}, // Q
    {0x7F, 0x09, 0x19, 0x29, 0x46}, // R
    {0x46, 0x49, 0x49, 0x49, 0x31}, // S
    {0x01, 0x01, 0x7F, 0x01, 0x01}, // T
    {0x3F, 0x40, 0x40, 0x40, 0x3F}, // U
    {0x1F, 0x20, 0x40, 0x20, 0x1F}, // V
    {0x7F, 0x20, 0x18, 0x20, 0x7F}, // W
    {0x63, 0x14, 0x08, 0x14, 0x63}, // X
    {0x03, 0x04, 0x78, 0x04, 0x03}, // Y
    {0x61, 0x51, 0x49, 0x45, 0x43}, // Z
    {0x00, 0x00, 0x7F, 0x41, 0x41}, // [
    {0x02, 0x04, 0x08, 0x10, 0x20}, // backslash
    {0x41, 0x41, 0x7F, 0x00, 0x00}, // ]
    {0x04, 0x02, 0x01, 0x02, 0x04}, // ^
    {0x40, 0x40, 0x40, 0x40, 0x40}, // _
    {0x00, 0x01, 0x02, 0x04, 0x00}, // `
    {0x20, 0x54, 0x54, 0x54, 0x78}, // a
    {0x7F, 0x48, 0x44, 0x44, 0x38}, // b
    {0x38, 0x44, 0x44, 0x44, 0x20}, // c
    {0x38, 0x44, 0x44, 0x48, 0x7F}, // d
    {0x38, 0x54, 0x54, 0x54, 0x18}, // e
    {0x08, 0x7E, 0x09, 0x01, 0x02}, // f
    {0x08, 0x14, 0x54, 0x54, 0x3C}, // g
    {0x7F, 0x08, 0x04, 0x04, 0x78}, // h
    {0x00, 0x44, 0x7D, 0x40, 0x00}, // i
    {0x20, 0x40, 0x44, 0x3D, 0x00}, // j
    {0x00, 0x7F, 0x10, 0x28, 0x44}, // k
    {0x00, 0x41, 0x7F, 0x40, 0x00}, // l
    {0x7C, 0x04, 0x18, 0x04, 0x78}, // m
    {0x7C, 0x08, 0x04, 0x04, 0x78}, // n
    {0x38, 0x44, 0x44, 0x44, 0x38}, // o
    {0x7C, 0x14, 0x14, 0x14, 0x08}, // p
    {0x08, 0x14, 0x14, 0x18, 0x7C}, // q
    {0x7C, 0x08, 0x04, 0x04, 0x08}, // r
    {0x48, 0x54, 0x54, 0x54, 0x20}, // s
    {0x04, 0x3F, 0x44, 0x40, 0x20}, // t
    {0x3C, 0x40, 0x40, 0x20, 0x7C}, // u
    {0x1C, 0x20, 0x40, 0x20, 0x1C}, // v
    {0x3C, 0x40, 0x30, 0x40, 0x3C}, // w
    {0x44, 0x28, 0x10, 0x28, 0x44}, // x
    {0x0C, 0x50, 0x50, 0x50, 0x3C}, // y
    {0x44, 0x64, 0x54, 0x4C, 0x44}, // z
    {0x00, 0x08, 0x36, 0x41, 0x00}, // {
    {0x00, 0x00, 0x7F, 0x00, 0x00}, // |
    {0x00, 0x41, 0x36, 0x08, 0x00}, // }
    {0x08, 0x04, 0x08, 0x10, 0x08}, // ~
    {0x00, 0x00, 0x00, 0x00, 0x00}  // DEL
};

// Article structure
typedef struct {
    const char* title;
    const char* content;
    const char* category;
} Article;

// Built-in articles
const Article articles[] = {
    { "Earth",
      "Mostly Harmless.\n\nEarth was a planet in the unfashionable end of the Western Spiral arm.\n\nDemolished for a hyperspace bypass.\n\nTrue purpose: organic computer to calculate the Ultimate Question.\n\nSTATUS: Demolished",
      "Planets"
    },
    { "Towel",
      "The most massively useful thing an interstellar hitchhiker can have.\n\nWrap it for warmth, lie on it on beaches, sleep under it, use in combat, ward off fumes, wave in emergencies, and dry yourself.\n\nMost importantly: psychological value. A hitchhiker with a towel is assumed to have everything else.\n\nSTATUS: Essential",
      "Essential Items"
    },
    { "42",
      "The Answer to the Ultimate Question of Life, the Universe, and Everything.\n\nAfter 7.5 million years, Deep Thought determined the Answer was 42.\n\nThe answer seemed meaningless because the beings never knew the Question.\n\nDeep Thought designed Earth to calculate the Ultimate Question.\n\nSIGNIFICANCE: Ultimate",
      "Philosophy"
    },
    { "Babel Fish",
      "Small, yellow, leech-like. Probably the oddest thing in the Universe.\n\nStick it in your ear to instantly understand any language.\n\nFeeds on brainwave energy and excretes a telepathic matrix into your mind.\n\nIts existence proves the non-existence of God.\n\nTYPE: Universal translator\n\nMETHOD: Insert in ear",
      "Technology"
    },
    { "Vogons",
      "One of the most unpleasant races in the Galaxy.\n\nNot evil, but bad-tempered, bureaucratic, officious and callous.\n\nWon't lift a finger without orders signed in triplicate, sent in, sent back, queried, lost, found, buried in peat and recycled.\n\nNEVER let a Vogon read poetry at you.\n\nPOETRY: Third worst\n\nTEMPERAMENT: Unpleasant",
      "Species"
    },
    { "Don't Panic",
      "Large, friendly letters on the cover of the Guide.\n\nThe most important advice for any interstellar traveler.\n\nNo matter how dire, how impossible, or how improbable...\n\nDON'T PANIC.\n\nAlways know where your towel is, keep your Guide handy, and above all:\n\nDON'T PANIC!",
      "Philosophy"
    },
    { "Heart of Gold",
      "Spaceship powered by the Infinite Improbability Drive.\n\nCan pass through every point in the Universe simultaneously.\n\nPowered by Bambleweeny 57 Sub-Meson Brain connected to atomic vector plotter.\n\nStolen at launch by Zaphod Beeblebrox.\n\nMakes highly improbable things occur in its vicinity.\n\nDRIVE: Infinite Improbability",
      "Spacecraft"
    },
    { "Pan Galactic Gargle Blaster",
      "The best drink in existence.\n\nLike having your brains smashed out by a slice of lemon wrapped round a large gold brick.\n\nInvented by Zaphod Beeblebrox.\n\nINGREDIENTS:\n- Ol' Janx Spirit\n- Santraginus V water\n- Arcturan Mega-gin\n- Fallian marsh gas\n- Algolian Suntiger tooth\n- Zamphour\n- Olive\n\nDANGER LEVEL: Extreme",
      "Beverages"
    },
    { "Zaphod Beeblebrox",
      "Two-headed, three-armed ex-President of the Galaxy.\n\nPart-time adventurer, full-time galactic hoopy frood.\n\nNotable achievements:\n- Survived the Total Perspective Vortex\n- Stole the Heart of Gold\n- Invented the Pan Galactic Gargle Blaster\n\nHis brain care-edit reveals he stole the ship on orders from himself.\n\nPERSONALITY: Cool\n\nHEADS: Two",
      "Personalities"
    },
    { "Marvin",
      "The Paranoid Android.\n\nPrototype GPP (Genuine People Personality) with a brain the size of a planet.\n\nPerpetually depressed due to massive intellect and menial tasks.\n\nNotable quotes:\n'Life? Don't talk to me about life.'\n\n'I think you ought to know I'm feeling very depressed.'\n\nHas terrible pain in all the diodes down his left side.\n\nDISPOSITION: Gloomy",
      "Personalities"
    },
    { "Ford Prefect",
      "Researcher for the Hitchhiker's Guide.\n\nFrom a small planet near Betelgeuse, not Guildford as he claimed.\n\nSpent 15 years on Earth researching before it was demolished. Contribution to the Guide: 'Mostly Harmless'.\n\nExpert at: Seeing the Universe for free, getting drunk, and not panicking.\n\nSKILLS: Hitchhiking\n\nORIGIN: Betelgeuse",
      "Personalities"
    },
    { "Arthur Dent",
      "Last surviving human from Earth (along with Trillian).\n\nRescued by Ford Prefect moments before Earth's demolition.\n\nPerpetually confused and looking for a proper cup of tea.\n\nNever quite grasped being shot into space. Kept expecting things to make sense.\n\nLast words on Earth:\n'This must be Thursday. I never could get the hang of Thursdays.'\n\nSTATUS: Bewildered",
      "Personalities"
    },
    { "Magrathea",
      "Ancient planet of legendary hyperspatial engineers.\n\nBuilt luxury planets for the wealthiest in the Universe during the Golden Age of prosperity.\n\nWent into hibernation when the economy collapsed.\n\nNotable creation: Earth (commissioned by mice as a supercomputer).\n\nSlartibartfast won an award for designing Norway's fjords.\n\nSTATUS: Dormant\n\nSPECIALTY: Planets",
      "Planets"
    },
    { "Slartibartfast",
      "Magrathean planet designer.\n\nSpecializes in coastlines and fjords. Won award for designing Norway.\n\nSomewhat embarrassed by his name, which sounds rather rude.\n\nHelped Arthur Dent understand Earth's true purpose as a computer.\n\nKnown for saying: 'I'd far rather be happy than right any day.'\n\nPROFESSION: Planet builder\n\nSPECIALTY: Fjords",
      "Personalities"
    },
    { "Deep Thought",
      "Second greatest computer in the Universe.\n\nBuilt to calculate the Ultimate Answer.\n\nAfter 7.5 million years, determined the Answer was 42.\n\nExplained the Answer seemed meaningless because no one knew the Question.\n\nDesigned Earth to calculate the Ultimate Question.\n\nNotable statement: 'I checked it very thoroughly and that quite definitely is the answer.'\n\nRANK: Second best",
      "Technology"
    },
    { "Trillian",
      "Astrophysicist, only other surviving human from Earth.\n\nReal name: Tricia McMillan.\n\nMet Arthur Dent at a party in Islington but left with Zaphod Beeblebrox instead.\n\nOne of the few beings to regularly make sense of the Universe.\n\nTraveled with Zaphod on the Heart of Gold.\n\nNoted for intelligence and patience with fools.\n\nOCCUPATION: Astrophysicist\n\nDISPOSITION: Sensible",
      "Personalities"
    },
    { "Infinite Improbability Drive",
      "Revolutionary starship propulsion breakthrough.\n\nPasses through every point in the Universe simultaneously.\n\nBased on principle: Given infinite improbability, any event is possible.\n\nSide effect: Creates highly improbable events around the ship.\n\nDiscovered by students at University of Maximegalon during a hot party.\n\nPowers the Heart of Gold.\n\nPROBABILITY: Infinite\n\nSIDE EFFECTS: Weird",
      "Technology"
    },
    { "Vogon Poetry",
      "Third worst poetry in the Universe.\n\nSecond worst: Azgoths of Kria. During recitation by Poet Master Grunthos the Flatulent, four died of internal hemorrhaging.\n\nWorst: Paula Nancy Millstone Jennings of Sussex (destroyed with Earth).\n\nVogon poetry used as torture method. Typical verse involves gruntbuggly and micturations.\n\nRECOMMENDATION: Avoid at all costs\n\nQUALITY: Dreadful",
      "Culture"
    },
    { "Milliways",
      "The Restaurant at the End of the Universe.\n\nBuilt on ruins of Frogstar World B, enclosed in time bubble at end of Universe.\n\nDiners watch the Universe end while enjoying meal.\n\nUniverse ends, time bubble resets, ready for next sitting.\n\nMax Quordlepleen provides commentary during the End of Everything.\n\nReservations essential.\n\nLOCATION: End of time\n\nSPECIALTY: The Big Crunch",
      "Locations"
    },
    { "Mice",
      "Hyper-intelligent pan-dimensional beings.\n\nCommissioned Earth as supercomputer to find the Ultimate Question.\n\nHumans were merely part of the computer's operating matrix.\n\nRan lab experiments on humans, who mistakenly believed they were the intelligent ones.\n\nFinal words before Earth's demolition: 'So long, and thanks for all the fish.'\n\nACTUAL ROLE: Programmers\n\nINTELLIGENCE: Hyper",
      "Species"
    }
};

const int num_articles = sizeof(articles) / sizeof(Article);

// State
int current_screen = 0; // 0=boot, 1=menu, 2=browse, 3=article, 4=search
int selected_article = 0;
int scroll_offset = 0;
uint8_t last_key = 0;

// Search state
char search_query[32] = "";
int search_query_len = 0;
int search_results[20];  // Indices of matching articles
int num_search_results = 0;
int selected_search_result = 0;

// Forward declarations
void init_display(void);
void init_keyboard(void);
void spi_write_command(uint8_t cmd);
void spi_write_data(uint8_t data);
void reset_controller(void);
void pico_lcd_init(void);
void lcd_clear(uint32_t color);
void lcd_pixel(int x, int y, uint32_t color);
void lcd_char(int x, int y, char c, uint32_t color);
void lcd_text(int x, int y, const char* str, uint32_t color);
void draw_boot_screen(void);
void draw_menu(void);
void draw_browse(void);
void draw_article(void);
void draw_search(void);
void perform_search(void);
uint8_t read_keyboard(void);
void handle_input(uint8_t key);
int strcasestr_simple(const char* haystack, const char* needle);

int main() {
    sleep_ms(200);
    
    init_display();
    init_keyboard();
    
    draw_boot_screen();
    sleep_ms(5000);
    
    current_screen = 1;
    draw_menu();
    
    while (1) {
        uint8_t key = read_keyboard();
        if (key != 0 && key != last_key) {
            handle_input(key);
            last_key = key;
        } else if (key == 0) {
            last_key = 0;
        }
        sleep_ms(50);
    }
    
    return 0;
}

void init_display(void) {
    gpio_init(LCD_CS);
    gpio_init(LCD_DC);
    gpio_init(LCD_RST);
    gpio_init(LCD_BL);
    
    gpio_set_dir(LCD_CS, GPIO_OUT);
    gpio_set_dir(LCD_DC, GPIO_OUT);
    gpio_set_dir(LCD_RST, GPIO_OUT);
    gpio_set_dir(LCD_BL, GPIO_OUT);
    
    spi_init(spi1, LCD_SPI_SPEED);
    gpio_set_function(LCD_SCK, GPIO_FUNC_SPI);
    gpio_set_function(LCD_MOSI, GPIO_FUNC_SPI);
    gpio_set_function(LCD_MISO, GPIO_FUNC_SPI);
    
    gpio_put(LCD_CS, 1);
    gpio_put(LCD_RST, 1);
    gpio_put(LCD_BL, 1);
    
    pico_lcd_init();
}

void init_keyboard(void) {
    i2c_init(KBD_I2C, 400 * 1000);
    gpio_set_function(KBD_SDA, GPIO_FUNC_I2C);
    gpio_set_function(KBD_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(KBD_SDA);
    gpio_pull_up(KBD_SCL);
}

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

    spi_write_command(0xE0);
    spi_write_data(0x00); spi_write_data(0x03); spi_write_data(0x09);
    spi_write_data(0x08); spi_write_data(0x16); spi_write_data(0x0A);
    spi_write_data(0x3F); spi_write_data(0x78); spi_write_data(0x4C);
    spi_write_data(0x09); spi_write_data(0x0A); spi_write_data(0x08);
    spi_write_data(0x16); spi_write_data(0x1A); spi_write_data(0x0F);

    spi_write_command(0XE1);
    spi_write_data(0x00); spi_write_data(0x16); spi_write_data(0x19);
    spi_write_data(0x03); spi_write_data(0x0F); spi_write_data(0x05);
    spi_write_data(0x32); spi_write_data(0x45); spi_write_data(0x46);
    spi_write_data(0x04); spi_write_data(0x0E); spi_write_data(0x0D);
    spi_write_data(0x35); spi_write_data(0x37); spi_write_data(0x0F);

    spi_write_command(0xC0);
    spi_write_data(0x17); spi_write_data(0x15);

    spi_write_command(0xC1);
    spi_write_data(0x41);

    spi_write_command(0xC5);
    spi_write_data(0x00); spi_write_data(0x12); spi_write_data(0x80);

    spi_write_command(0x36);
    spi_write_data(0x48);

    spi_write_command(0x3A);
    spi_write_data(0x66);

    spi_write_command(0xB0);
    spi_write_data(0x00);

    spi_write_command(0xB1);
    spi_write_data(0xA0);

    spi_write_command(0x21);

    spi_write_command(0xB4);
    spi_write_data(0x02);

    spi_write_command(0xB6);
    spi_write_data(0x02); spi_write_data(0x02); spi_write_data(0x3B);

    spi_write_command(0xB7);
    spi_write_data(0xC6);
    
    spi_write_command(0xE9);
    spi_write_data(0x00);

    spi_write_command(0xF7);
    spi_write_data(0xA9); spi_write_data(0x51);
    spi_write_data(0x2C); spi_write_data(0x82);

    spi_write_command(0x11);
    sleep_ms(120);

    spi_write_command(0x29);
    sleep_ms(120);
}

void lcd_clear(uint32_t color) {
    spi_write_command(0x2A);
    spi_write_data(0); spi_write_data(0);
    spi_write_data(0x01); spi_write_data(0x3F);
    
    spi_write_command(0x2B);
    spi_write_data(0); spi_write_data(0);
    spi_write_data(0x01); spi_write_data(0x3F);
    
    spi_write_command(0x2C);
    
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

void lcd_pixel(int x, int y, uint32_t color) {
    if (x < 0 || x >= 320 || y < 0 || y >= 320) return;
    
    spi_write_command(0x2A);
    spi_write_data(x >> 8); spi_write_data(x & 0xFF);
    spi_write_data(x >> 8); spi_write_data(x & 0xFF);
    
    spi_write_command(0x2B);
    spi_write_data(y >> 8); spi_write_data(y & 0xFF);
    spi_write_data(y >> 8); spi_write_data(y & 0xFF);
    
    spi_write_command(0x2C);
    
    uint8_t rgb[3];
    rgb[0] = (color >> 16) & 0xFF;
    rgb[1] = (color >> 8) & 0xFF;
    rgb[2] = color & 0xFF;
    
    gpio_put(LCD_DC, 1);
    gpio_put(LCD_CS, 0);
    spi_write_blocking(spi1, rgb, 3);
    gpio_put(LCD_CS, 1);
}

void lcd_line(int x0, int y0, int x1, int y1, uint32_t color) {
    int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = dx + dy, e2;

    while (1) {
        lcd_pixel(x0, y0, color);
        if (x0 == x1 && y0 == y1) break;
        e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
    }
}

void lcd_rect(int x, int y, int w, int h, uint32_t color) {
    lcd_line(x, y, x + w, y, color);
    lcd_line(x + w, y, x + w, y + h, color);
    lcd_line(x + w, y + h, x, y + h, color);
    lcd_line(x, y + h, x, y, color);
}

void lcd_circle(int xc, int yc, int r, uint32_t color) {
    int x = 0, y = r;
    int d = 3 - 2 * r;
    while (y >= x) {
        lcd_pixel(xc + x, yc + y, color);
        lcd_pixel(xc - x, yc + y, color);
        lcd_pixel(xc + x, yc - y, color);
        lcd_pixel(xc - x, yc - y, color);
        lcd_pixel(xc + y, yc + x, color);
        lcd_pixel(xc - y, yc + x, color);
        lcd_pixel(xc + y, yc - x, color);
        lcd_pixel(xc - y, yc - x, color);
        x++;
        if (d > 0) {
            y--;
            d = d + 4 * (x - y) + 10;
        } else {
            d = d + 4 * x + 6;
        }
    }
}

void lcd_fill_rect(int x, int y, int w, int h, uint32_t color) {
    if (x < 0) { w += x; x = 0; }
    if (y < 0) { h += y; y = 0; }
    if (x + w > LCD_WIDTH) w = LCD_WIDTH - x;
    if (y + h > LCD_HEIGHT) h = LCD_HEIGHT - y;
    if (w <= 0 || h <= 0) return;
    
    for (int cy = y; cy < y + h; cy++) {
        for (int cx = x; cx < x + w; cx++) {
            lcd_pixel(cx, cy, color);
        }
    }
}

void lcd_fill_circle(int xc, int yc, int r, uint32_t color) {
    for (int y = -r; y <= r; y++) {
        for (int x = -r; x <= r; x++) {
            if (x * x + y * y <= r * r) {
                lcd_pixel(xc + x, yc + y, color);
            }
        }
    }
}

void lcd_hline(int x0, int x1, int y, uint32_t color) {
    if (x0 > x1) { int tmp = x0; x0 = x1; x1 = tmp; }
    for (int x = x0; x <= x1; x++) {
        lcd_pixel(x, y, color);
    }
}

void lcd_vline(int x, int y0, int y1, uint32_t color) {
    if (y0 > y1) { int tmp = y0; y0 = y1; y1 = tmp; }
    for (int y = y0; y <= y1; y++) {
        lcd_pixel(x, y, color);
    }
}

void lcd_char(int x, int y, char c, uint32_t color) {
    if (c < 32 || c > 126) return;
    
    for (int col = 0; col < 5; col++) {
        uint8_t line = font5x7[c - 32][col];
        for (int row = 0; row < 7; row++) {
            if (line & (1 << row)) {
                lcd_pixel(x + col, y + row, color);
            }
        }
    }
}

void lcd_text(int x, int y, const char* str, uint32_t color) {
    int cx = x;
    int cy = y;
    while (*str) {
        if (*str == '\n') {
            cx = x;
            cy += 10;
        } else {
            lcd_char(cx, cy, *str, color);
            cx += 6;
            if (cx > 314) {
                cx = x;
                cy += 10;
            }
        }
        str++;
    }
}

void draw_boot_screen(void) {
    // Enhanced boot screen
    lcd_clear(COLOR_BLACK);
    
    // Draw compressed image
    draw_compressed_image(dont_panic_compressed, dont_panic_size);
    
    // Enhanced version overlay with gradient background
    draw_gradient_rect(0, 280, 320, 40, COLOR_HGTTG_DARK, COLOR_BLACK, true);
    draw_outlined_text(20, 290, "v2.1 Enhanced - Paul Wyman - 2025", COLOR_HGTTG_BRIGHT, COLOR_BLACK, 1);
    
    // Add loading animation
    for (int frame = 0; frame < 20; frame++) {
        draw_loading_animation(270, 290, frame);
        sleep_ms(100);
    }
}

void draw_menu(void) {
    lcd_clear(COLOR_BLACK);
    
    // Enhanced header with gradient
    draw_gradient_rect(0, 0, 320, 60, COLOR_HGTTG_DARK, COLOR_HGTTG_MEDIUM, true);
    draw_outlined_text(10, 10, "THE HITCHHIKER'S GUIDE", COLOR_HGTTG_BRIGHT, COLOR_BLACK, 2);
    draw_outlined_text(10, 35, "TO THE GALAXY", COLOR_AMBER_BRIGHT, COLOR_BLACK, 1);
    
    // Enhanced menu items
    draw_menu_item(20, 80, "Browse Articles", 1, false);
    draw_menu_item(20, 110, "Search Articles", 2, false);
    draw_menu_item(20, 140, "Random Article", 3, false);
    draw_menu_item(20, 170, "About", 4, false);
    
    // Footer with better styling
    draw_rounded_rect(10, 280, 200, 25, 5, COLOR_AMBER_DARK);
    lcd_text(15, 288, "Press number key to select", COLOR_YELLOW_BRIGHT);
    
    // DON'T PANIC reminder
    draw_outlined_text(230, 250, "DON'T", COLOR_HGTTG_BRIGHT, COLOR_HGTTG_DARK, 2);
    draw_outlined_text(230, 275, "PANIC", COLOR_HGTTG_BRIGHT, COLOR_HGTTG_DARK, 2);
}

void draw_browse(void) {
    lcd_clear(COLOR_BLACK);
    
    // Enhanced header
    draw_article_header("ARTICLE BROWSER", "Library");
    
    int y = 60;
    for (int i = 0; i < num_articles && i < 9; i++) {
        bool is_selected = (i == selected_article);
        uint32_t bg_color = is_selected ? COLOR_HGTTG_MEDIUM : COLOR_BLACK;
        uint32_t fg_color = is_selected ? COLOR_BLACK : COLOR_HGTTG_BRIGHT;
        
        if (is_selected) {
            draw_rounded_rect(10, y - 2, 280, 22, 5, bg_color);
        }
        
        char line[50];
        snprintf(line, sizeof(line), "%c %s", 
                 is_selected ? '>' : ' ',
                 articles[i].title);
        
        draw_large_text(15, y + 2, line, fg_color, 1);
        
        // Show category
        lcd_text(200, y + 5, articles[i].category, COLOR_AMBER_MEDIUM);
        
        y += 25;
    }
    
    // Enhanced footer
    draw_rounded_rect(10, 280, 200, 25, 5, COLOR_AMBER_DARK);
    lcd_text(15, 288, "↑↓ Navigate  ENTER Select  ESC Back", COLOR_YELLOW_BRIGHT);
    
    // Scroll indicator
    draw_scroll_indicator(300, 60, num_articles, 9, selected_article);
}

// Draw text character by character like a teleprinter
int lcd_text_teleprinter_scroll(int x, int y, const char* str, uint32_t color, int delay_ms, int scroll_offset, int max_visible_lines) {
    int cx = x;
    int cy = y;
    int usable_width = 320 - x - 5;
    int max_cx = x + usable_width;

    int line_num = 0;     // current line number
    int total_lines = 0;  // total number of lines rendered

    while (*str) {
        char c = *str;
        if (c >= 'a' && c <= 'z') c -= 32; // uppercase

        if (c == '\n') {
            line_num++;
            cy += 12;
            cx = x;
        } else {
            if (line_num >= scroll_offset && line_num < scroll_offset + max_visible_lines) {
                lcd_char(cx, cy - (scroll_offset * 12), c, color);
            }
            cx += 7;
            if (cx + 5 >= max_cx) {
                line_num++;
                cy += 12;
                cx = x;
            }
        }
        str++;
    }

    // Return total number of lines (for scroll bounds)
    return line_num + 1;
}

void draw_article(void) {
    lcd_clear(COLOR_BLACK);

    const Article* art = &articles[selected_article];

    // Enhanced article header
    draw_article_header(art->title, art->category);

    // === Enhanced Diagram area ===
    draw_rounded_rect(10, 55, 300, 100, 8, COLOR_BLACK);
    lcd_rect(10, 55, 300, 100, COLOR_CYAN_MEDIUM);
    
    // Draw appropriate diagram based on article
    if (strcmp(art->title, "Babel Fish") == 0) {
        draw_babel_fish_diagram(15, 60);
    } else if (strcmp(art->title, "Earth") == 0) {
        draw_earth_diagram(15, 60);
    } else if (strcmp(art->title, "Towel") == 0) {
        draw_towel_diagram(15, 60);
    } else if (strcmp(art->title, "Vogons") == 0) {
        draw_vogon_diagram(15, 60);
    } else if (strcmp(art->title, "Heart of Gold") == 0) {
        draw_heart_of_gold_diagram(15, 60);
    } else if (strcmp(art->title, "Zaphod Beeblebrox") == 0) {
        draw_zaphod_diagram(15, 60);
    } else if (strcmp(art->title, "Marvin") == 0) {
        draw_marvin_diagram(15, 60);
    } else if (strcmp(art->title, "42") == 0) {
        draw_42_diagram(15, 60);
    } else if (strcmp(art->title, "Don't Panic") == 0) {
        draw_dont_panic_diagram(15, 60);
    } else if (strcmp(art->title, "Pan Galactic Gargle Blaster") == 0) {
        draw_pan_galactic_diagram(15, 60);
    } else {
        // Generic diagram for other articles
        draw_gradient_rect(15, 60, 290, 90, COLOR_PURPLE_MEDIUM, COLOR_BLUE_BRIGHT, true);
        draw_large_text(50, 85, "GUIDE ENTRY", COLOR_YELLOW_BRIGHT, 2);
        lcd_text(80, 110, "No specific diagram available", COLOR_CYAN_BRIGHT);
        lcd_text(90, 125, "Content follows below", COLOR_CYAN_MEDIUM);
    }

    // === Teleprinter article content ===
    // Starts below diagram (y ≈ 145)
    int max_visible_lines = 12; // ~12 lines fit below diagram (~150px to ~300px)
int total_lines = lcd_text_teleprinter_scroll(10, 150, art->content, COLOR_BLUE, 0, scroll_offset, max_visible_lines);

// Clamp scroll_offset so we don’t scroll past the end
if (scroll_offset > total_lines - max_visible_lines) {
    scroll_offset = total_lines - max_visible_lines;
    if (scroll_offset < 0) scroll_offset = 0;
}



    // Footer
    lcd_text(10, 300, "UP/DOWN ESC", COLOR_GREEN);
}








// Case-insensitive substring search
int strcasestr_simple(const char* haystack, const char* needle) {
    if (!*needle) return 1; // Empty needle matches
    
    int needle_len = strlen(needle);
    int haystack_len = strlen(haystack);
    
    for (int i = 0; i <= haystack_len - needle_len; i++) {
        int match = 1;
        for (int j = 0; j < needle_len; j++) {
            char h = haystack[i + j];
            char n = needle[j];
            // Convert to lowercase for comparison
            if (h >= 'A' && h <= 'Z') h = h + 32;
            if (n >= 'A' && n <= 'Z') n = n + 32;
            if (h != n) {
                match = 0;
                break;
            }
        }
        if (match) return 1;
    }
    return 0;
}

void perform_search(void) {
    num_search_results = 0;
    
    if (search_query_len == 0) {
        // Empty search shows all articles
        for (int i = 0; i < num_articles && num_search_results < 20; i++) {
            search_results[num_search_results++] = i;
        }
    } else {
        // Filter by search query
        for (int i = 0; i < num_articles && num_search_results < 20; i++) {
            if (strcasestr_simple(articles[i].title, search_query) ||
                strcasestr_simple(articles[i].category, search_query)) {
                search_results[num_search_results++] = i;
            }
        }
    }
    
    selected_search_result = 0;
}

void draw_search(void) {
    lcd_clear(COLOR_BLACK);
    
    // Enhanced search header
    draw_article_header("SEARCH ENGINE", "Query");
    
    // Enhanced search box with border
    draw_rounded_rect(10, 60, 280, 25, 5, COLOR_HGTTG_DARK);
    lcd_rect(10, 60, 280, 25, COLOR_HGTTG_BRIGHT);
    
    char search_display[34];
    snprintf(search_display, sizeof(search_display), "> %s_", search_query);
    draw_large_text(15, 67, search_display, COLOR_HGTTG_BRIGHT, 1);
    
    // Enhanced results count with icon
    char count[32];
    snprintf(count, sizeof(count), "Found: %d articles", num_search_results);
    draw_rounded_rect(10, 90, 150, 18, 3, COLOR_AMBER_DARK);
    lcd_text(15, 95, count, COLOR_YELLOW_BRIGHT);
    
    // Enhanced matching articles list
    int y = 115;
    int visible_results = (num_search_results < 7) ? num_search_results : 7;
    
    for (int i = 0; i < visible_results; i++) {
        int article_idx = search_results[i];
        bool is_selected = (i == selected_search_result);
        
        if (is_selected) {
            draw_rounded_rect(10, y - 2, 280, 22, 5, COLOR_HGTTG_MEDIUM);
        }
        
        uint32_t color = is_selected ? COLOR_BLACK : COLOR_HGTTG_BRIGHT;
        char line[50];
        snprintf(line, sizeof(line), "%c %s", 
                 is_selected ? '>' : ' ',
                 articles[article_idx].title);
        draw_large_text(15, y + 2, line, color, 1);
        
        // Show category for each result
        lcd_text(200, y + 5, articles[article_idx].category, COLOR_AMBER_MEDIUM);
        
        y += 25;
    }
    
    // Enhanced footer with instructions
    draw_rounded_rect(10, 280, 280, 25, 5, COLOR_AMBER_DARK);
    lcd_text(15, 288, "Type/Del/Enter Select  ESC Back", COLOR_YELLOW_BRIGHT);
    
    // Search results scroll indicator
    if (num_search_results > 7) {
        draw_scroll_indicator(300, 115, num_search_results, 7, selected_search_result);
    }
}

uint8_t read_keyboard(void) {
    uint16_t buff = 0;
    uint8_t msg[2];
    
    // Send command 0x09 to request keyboard data
    msg[0] = 0x09;
    int ret = i2c_write_timeout_us(KBD_I2C, KBD_ADDR, msg, 1, false, 500000);
    if (ret < 0) return 0;
    
    // Wait 16ms as per protocol
    sleep_ms(16);
    
    // Read 2 bytes
    ret = i2c_read_timeout_us(KBD_I2C, KBD_ADDR, (uint8_t*)&buff, 2, false, 500000);
    if (ret < 0) return 0;
    
    // Check if key pressed (bit 0 of low byte is 1)
    if (buff != 0 && (buff & 0xFF) == 1) {
        // Key code is in high byte
        return (buff >> 8) & 0xFF;
    }
    
    return 0;
}

void handle_input(uint8_t key) {
    if (current_screen == 1) { // Menu
        if (key == '1') {
            current_screen = 2;
            selected_article = 0;
            draw_browse();
        } else if (key == '2') {
            current_screen = 4;
            search_query[0] = '\0';
            search_query_len = 0;
            perform_search();
            draw_search();
        } else if (key == '3') {
            current_screen = 3;
            selected_article = rand() % num_articles;
            scroll_offset = 0;
            draw_article();
        } else if (key == '4') {
            lcd_clear(COLOR_DARK);
            lcd_text(10, 50, "Hitchhiker's Guide", COLOR_HGTTG);
            lcd_text(10, 70, "PicoCalc Edition", COLOR_AMBER);
            lcd_text(10, 100, "v42.0", COLOR_GRAY);
            lcd_text(10, 130, "DON'T PANIC!", COLOR_HGTTG);
            sleep_ms(3000);
            draw_menu();
        }
    } else if (current_screen == 2) { // Browse
        if (key == 0xB1) { // ESC
            current_screen = 1;
            draw_menu();
        } else if (key == 0xB5 || key == 'w' || key == 'k') { // Up
            if (selected_article > 0) {
                selected_article--;
                draw_browse();
            }
        } else if (key == 0xB6 || key == 's' || key == 'j') { // Down
            if (selected_article < num_articles - 1) {
                selected_article++;
                draw_browse();
            }
        } else if (key == '\n' || key == '\r' || key == ' ') { // Enter
            current_screen = 3;
            scroll_offset = 0;
            draw_article();
        }
    } else if (current_screen == 3) { // Article
        if (key == 0xB1) { // ESC
            current_screen = 2;
            draw_browse();
        } else if (key == 0xB5 || key == 'w' || key == 'k') { // Up
            if (scroll_offset > 0) {
                scroll_offset--;
                draw_article();
            }
        } else if (key == 0xB6 || key == 's' || key == 'j') { // Down
            scroll_offset++;
            draw_article();
        }
    } else if (current_screen == 4) { // Search
        if (key == 0xB1) { // ESC
            current_screen = 1;
            draw_menu();
        } else if (key == 0xB5 || key == 'w' || key == 'k') { // Up
            if (selected_search_result > 0) {
                selected_search_result--;
                draw_search();
            }
        } else if (key == 0xB6 || key == 's' || key == 'j') { // Down
            if (selected_search_result < num_search_results - 1 && 
                selected_search_result < 8) {
                selected_search_result++;
                draw_search();
            }
        } else if (key == '\n' || key == '\r') { // Enter
            if (num_search_results > 0) {
                selected_article = search_results[selected_search_result];
                current_screen = 3;
                scroll_offset = 0;
                draw_article();
            }
        } else if (key == 0x08 || key == 0x7F || key == 0xB2) { // Backspace/Delete
            if (search_query_len > 0) {
                search_query_len--;
                search_query[search_query_len] = '\0';
                perform_search();
                draw_search();
            }
        } else if ((key >= 'a' && key <= 'z') || 
                   (key >= 'A' && key <= 'Z') || 
                   (key >= '0' && key <= '9') || 
                   key == ' ') {
            if (search_query_len < 30) {
                search_query[search_query_len++] = key;
                search_query[search_query_len] = '\0';
                perform_search();
                draw_search();
            }
        }
    }
}