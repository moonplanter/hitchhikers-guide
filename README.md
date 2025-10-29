# Hitchhiker's Guide to the Galaxy - PicoCalc Firmware

<img width="320" height="234" alt="image" src="https://github.com/user-attachments/assets/77ffd427-e5cf-47c5-95db-7b8c8e837897" />


A complete Hitchhiker's Guide implementation for the ClockworkPi PicoCalc.

## Features

- **"DON'T PANIC" boot screen** - Because it's essential
- **Article browsing** - Navigate through Guide entries
- **Search functionality** - Find specific topics
- **Random article** - Discover something new
- **Classic aesthetic** - Green/amber text on dark background
- **SD card storage** - Unlimited articles

## Hardware Requirements

- ClockworkPi PicoCalc
- 18650 battery
- MicroSD card (formatted FAT32)

## Build Instructions

### Prerequisites

1. Install the Pico SDK:
```bash
git clone https://github.com/raspberrypi/pico-sdk.git
cd pico-sdk
git submodule update --init
export PICO_SDK_PATH=$(pwd)
```

2. Install build tools:
```bash
# Ubuntu/Debian
sudo apt install cmake gcc-arm-none-eabi libnewlib-arm-none-eabi build-essential

# macOS
brew install cmake
brew tap ArmMbed/homebrew-formulae
brew install arm-none-eabi-gcc
```

### Building the Firmware

1. Clone this repository:
```bash
git clone https://github.com/yourusername/hgttg-picocalc.git
cd hgttg-picocalc
```

2. Create build directory:
```bash
mkdir build
cd build
```

3. Configure and build:
```bash
cmake ..
make
```

4. The output file `hitchhikers_guide.uf2` will be in the build directory.

## Installing to PicoCalc

1. **Flash the firmware:**
   - Hold the BOOTSEL button on the Pico while connecting USB
   - The Pico will appear as a USB mass storage device
   - Copy `hitchhikers_guide.uf2` to the drive
   - The device will automatically reboot

2. **Prepare the SD card:**
   - Format as FAT32
   - Create the following structure:

```
SD_CARD/
└── guide/
    ├── index.txt
    ├── earth.txt
    ├── towel.txt
    ├── forty_two.txt
    ├── babel_fish.txt
    ├── vogons.txt
    └── [more articles...]
```

3. **Add the article files:**
   - Copy `index.txt` and all article `.txt` files to the `guide/` folder
   - Insert the SD card into the PicoCalc

## Creating Custom Articles

Articles are simple text files. Format:

```
Title text here

Body paragraph 1...

Body paragraph 2...

STATUS: [status]
LEVEL: [level]
```

Add new entries to `index.txt`:
```
Article Title|filename.txt|Category
```

## Controls
![9AEE17F8-ED5B-4913-96DF-222DC34666E4_1_201_a](https://github.com/user-attachments/assets/22b42808-4039-41b5-ad08-b750a7bd6161)

### Main Menu
- **1-4** - Select menu option
- **ESC** - Exit/back

- 
![34C742CE-E555-4BD1-8972-14C4531012E5_1_201_a](https://github.com/user-attachments/assets/3655b627-5f96-4ea8-a9ec-dafa9b89027a)

### Browse Mode
- **↑/↓** - Navigate articles
- **Enter** - Open article
- **ESC** - Return to main menu

- 
![6A1694F4-F27A-42E4-91D5-1828C2AB9469_1_201_a](https://github.com/user-attachments/assets/e75c3306-64cb-4c1d-bdcf-0609a9e7bfa3)

### Article View
- **↑/↓** - Scroll content
- **ESC** - Return to browse

### Search Mode
- **Letters** - Type search term
- **Enter** - Execute search
- **ESC** - Cancel

## Customization

### Colors
Edit the color definitions in `main.c`:
```c
#define COLOR_HGTTG    0x07E0  // Bright green
#define COLOR_AMBER    0xFD20  // Amber/orange
#define COLOR_DARK_BG  0x0841  // Very dark green
```
![9B6E7E27-DBB7-4825-93A7-905B2D6FC41D_1_201_a](https://github.com/user-attachments/assets/4546ee2f-4e9a-4156-827f-e49ee5cbc395)

### Boot Screen
Modify `draw_boot_screen()` function to customize the startup animation.

### Add Easter Eggs
Insert special triggers in `handle_keyboard()` - for example, type "42" anywhere for a special message!

## Adding Font Support

The current code has placeholder text rendering. To add proper fonts:

1. **Option A: Use u8g2 library**
```bash
cd lib
git clone https://github.com/olikraus/u8g2.git
```

Then add to CMakeLists.txt:
```cmake
add_subdirectory(lib/u8g2)
target_link_libraries(hitchhikers_guide u8g2)
```

2. **Option B: Use bitmap fonts**
   - Create a simple 8x16 bitmap font array
   - Include in your code as a header file
   - Render character by character

Example font rendering:
```c
void lcd_draw_char(int x, int y, char c, uint16_t color) {
    // Draw 8x16 character from font bitmap
    for (int row = 0; row < 16; row++) {
        uint8_t line = font_8x16[c * 16 + row];
        for (int col = 0; col < 8; col++) {
            if (line & (1 << col)) {
                lcd_draw_pixel(x + col, y + row, color);
            }
        }
    }
}
```

## Adding FatFS for SD Card Support

1. Download FatFS:
```bash
mkdir -p lib
cd lib
wget http://elm-chan.org/fsw/ff/arc/ff15.zip
unzip ff15.zip -d FatFs
```

2. Configure FatFS:
   - Edit `ffconf.h` for your needs
   - Implement SPI SD card driver functions
   - Add to CMakeLists.txt

3. Required functions to implement:
   - `disk_initialize()`
   - `disk_status()`
   - `disk_read()`
   - `disk_write()`
   - `disk_ioctl()`
   - `get_fattime()`

## Project Structure

```
hitchhikers-guide/
├── main.c                 # Main application
├── CMakeLists.txt        # Build configuration
├── README.md             # This file
├── lib/                  # External libraries
│   ├── FatFs/           # SD card filesystem
│   └── u8g2/            # Font library (optional)
├── sd_card/             # Files for SD card
│   └── guide/
│       ├── index.txt    # Article database
│       └── *.txt        # Article files
└── docs/                # Additional documentation
```

## Sample Articles Included

- **Earth** - Mostly Harmless
- **Towel** - The most useful item
- **42** - The Ultimate Answer
- **Babel Fish** - Universal translator
- **Vogons** - Worst poetry in the universe
- **Pan Galactic Gargle Blaster** - Best drink in existence
- **Heart of Gold** - Ship with Infinite Improbability Drive

## Future Enhancements

### Phase 2 Features
- [ ] Search with fuzzy matching
- [ ] Bookmarks
- [ ] History tracking
- [ ] Article links/references
- [ ] Image support for diagrams
- [ ] Sound effects
- [ ] WiFi integration for live updates

### Phase 3 Features
- [ ] Marvin personality mode (pessimistic responses)
- [ ] Random quote generator
- [ ] "Infinite Improbability" mode - random article jumps
- [ ] Babel fish translator (for different languages)
- [ ] Integration with actual Guide API if available

## Easter Eggs

Some hidden features to discover:
- Type "42" anywhere
- Press certain key combinations
- Look for special article titles
- Check the date on certain days

*"The ships hung in the sky in much the same way that bricks don't."*

## Troubleshooting

### Display issues
- Check SPI connections
- Verify pin definitions match your hardware
- Test with simple color fills first

### Keyboard not responding
- Verify I2C address (should be 0x55)
- Check pull-up resistors
- Test I2C bus with simple scan

### SD card not detected
- Ensure FAT32 format
- Check SPI connections
- Verify SD card is compatible (Class 10 recommended)
- Try different card if issues persist

### Build errors
- Verify PICO_SDK_PATH is set correctly
- Check all dependencies are installed
- Ensure ARM toolchain is in PATH

## Contributing

Feel free to add more articles, improve the UI, or add features!

### Adding Articles
1. Create `.txt` file in `sd_card/guide/`
2. Add entry to `index.txt`
3. Follow the article format guidelines
4. Test on device
5. Submit PR with your additions

### Suggested Article Topics
- More planets and locations
- Additional characters
- Technology items
- Cultural references
- Restaurants and locations
- Species and creatures
- Spaceships and vehicles

## Credits

Based on "The Hitchhiker's Guide to the Galaxy" by Douglas Adams.

Hardware by ClockworkPi.

Firmware created for educational and entertainment purposes.

## License

This firmware is released under GPL v3, consistent with ClockworkPi's licensing.

The Hitchhiker's Guide to the Galaxy is copyright Douglas Adams.

---

## Quick Start Checklist

- [ ] Pico SDK installed
- [ ] ARM toolchain installed  
- [ ] Repository cloned
- [ ] Firmware built successfully
- [ ] UF2 file flashed to PicoCalc
- [ ] SD card formatted (FAT32)
- [ ] Guide files copied to SD card
- [ ] SD card inserted
- [ ] Battery installed
- [ ] Power on and see "DON'T PANIC"

**Remember: DON'T PANIC!**
