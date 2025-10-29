# Hitchhiker's Guide PicoCalc - Quick Start

## What You've Got Here

A complete firmware package to turn your ClockworkPi PicoCalc into a fully functional Hitchhiker's Guide to the Galaxy! Perfect for showing off at the HGTTG conference.

## What's Included

### Core Firmware Files
- **main.c** - Main application with state machine, article browsing, search
- **display.c/h** - Enhanced display functions with UI helpers
- **font.h** - 8x8 bitmap font for that retro terminal look
- **CMakeLists.txt** - Build configuration
- **build.sh** - Easy build script

### Content Files
- **index.txt** - Article database index
- **Sample articles**: Earth, Towel, 42, Babel Fish, Vogons, etc.
- **article_manager.py** - Python tool to create/manage articles

### Documentation
- **README.md** - Complete documentation
- **QUICKSTART.md** - This file!

## 5-Minute Setup

### 1. Prerequisites (One Time)

```bash
# Install Pico SDK
git clone https://github.com/raspberrypi/pico-sdk.git ~/pico-sdk
cd ~/pico-sdk
git submodule update --init
export PICO_SDK_PATH=~/pico-sdk

# Add to ~/.bashrc for permanent:
echo 'export PICO_SDK_PATH=~/pico-sdk' >> ~/.bashrc

# Install tools (Ubuntu/Debian)
sudo apt install cmake gcc-arm-none-eabi libnewlib-arm-none-eabi build-essential

# Install tools (macOS)
brew install cmake
brew tap ArmMbed/homebrew-formulae
brew install arm-none-eabi-gcc
```

### 2. Build Firmware

```bash
chmod +x build.sh
./build.sh
```

Output: `build/hitchhikers_guide.uf2`

### 3. Flash to PicoCalc

1. Hold **BOOTSEL** button on the Pico
2. Connect USB cable (while holding BOOTSEL)
3. PicoCalc appears as USB drive
4. Drag `hitchhikers_guide.uf2` to the drive
5. Device reboots automatically

### 4. Prepare SD Card

```bash
# Format SD card as FAT32
# Then copy content files:
mkdir /path/to/sdcard/guide
cp sd_card/guide/* /path/to/sdcard/guide/
```

### 5. Power On!

Insert SD card, install 18650 battery, power on, and enjoy the "DON'T PANIC" boot screen!

## Adding More Articles

### Method 1: Using Python Tool

```bash
# Create new article interactively
python3 article_manager.py create

# List all articles
python3 article_manager.py list

# Search for articles
python3 article_manager.py search robot

# View an article
python3 article_manager.py view "Marvin"

# Check for issues
python3 article_manager.py validate

# Show statistics
python3 article_manager.py stats
```

### Method 2: Manually

1. Create `article_name.txt` in `sd_card/guide/`
2. Write your content (plain text, ~40 chars per line recommended)
3. Add entry to `sd_card/guide/index.txt`:
   ```
   Article Title|article_name.txt|Category
   ```
4. Copy to SD card

## Conference Tips

### Demo Features
1. **Boot animation** - "DON'T PANIC" screen
2. **Browse mode** - Navigate with arrow keys
3. **Random article** - Press 3 from main menu
4. **Search** - Press 2 (if implemented)

### Suggested Articles for Conference
- Douglas Adams biography
- History of HGTTG
- Book publication timeline
- Radio show information
- Movie and TV adaptations
- Fan community info

### Easter Eggs to Add
- Type "42" anywhere for special message
- Hold certain keys during boot
- Special responses on specific dates
- Hidden "Infinite Improbability" mode

## Customization

### Change Colors
Edit `display.h`:
```c
#define COLOR_HGTTG    0x07E0  // Your color here
#define COLOR_AMBER    0xFD20  // Your color here
#define COLOR_DARK_BG  0x0841  // Your color here
```

### Modify Boot Screen
Edit `draw_boot_screen()` in `main.c`

### Add Sound Effects
If you have a speaker:
- Boot sound
- Key press clicks
- Article open sound

## Troubleshooting

### "Build failed"
- Check `PICO_SDK_PATH` is set
- Verify ARM toolchain installed
- Try: `cd build && make clean && make`

### "Display not working"
- Verify pin definitions in `main.c`
- Check SPI connections
- Test with simple `lcd_clear()` first

### "SD card not reading"
- Must be FAT32 format
- Check folder is named "guide" (lowercase)
- Verify index.txt format (pipe-separated)

### "Keyboard not responding"
- Check I2C address (0x55)
- Verify pull-up resistors
- Test I2C with simple scan

## Next Steps

### Phase 2 Enhancements
- [ ] Implement search functionality
- [ ] Add bookmarks
- [ ] Article cross-references
- [ ] Images/diagrams support
- [ ] History tracking

### Phase 3 Features
- [ ] WiFi for live updates
- [ ] Marvin personality mode
- [ ] Quote of the day
- [ ] Babel fish translator
- [ ] Games (like the text adventure)

## Resources

- **ClockworkPi Forum**: https://forum.clockworkpi.com
- **PicoCalc GitHub**: https://github.com/clockworkpi/PicoCalc
- **Pico SDK Docs**: https://raspberrypi.github.io/pico-sdk-doxygen/
- **HGTTG Wiki**: https://hitchhikers.fandom.com

## Show It Off!

At the conference:
1. Boot it up - everyone loves "DON'T PANIC"
2. Show the article browsing
3. Demo random article feature
4. Let people add their own articles
5. Maybe add conference-specific content!

## Files Checklist

Before the conference, make sure you have:
- [x] Firmware compiled (`hitchhikers_guide.uf2`)
- [x] SD card formatted and loaded
- [x] At least 10-15 articles ready
- [x] Battery charged
- [x] Backup battery
- [x] USB cable for demos
- [x] Source code on laptop (for showing)

## Final Notes

The code is documented and designed to be easy to modify. Feel free to:
- Add more articles (Douglas Adams has so much material!)
- Customize the UI
- Add features
- Share with other fans

Most importantly: **DON'T PANIC** if something doesn't work perfectly. It's all part of the journey!

## License

GPL v3 (consistent with ClockworkPi)

Content inspired by Douglas Adams' works.

---

**Now go forth and demonstrate to the universe that a towel is, indeed, about the most massively useful thing an interstellar hitchhiker can have!**

*"So long, and thanks for all the fish!"*