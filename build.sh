#!/bin/bash
# Build script for Hitchhiker's Guide PicoCalc firmware

set -e

# Colors for output
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
NC='\033[0m' # No Color

echo -e "${GREEN}========================================${NC}"
echo -e "${GREEN}Hitchhiker's Guide to the Galaxy${NC}"
echo -e "${GREEN}PicoCalc Firmware Builder${NC}"
echo -e "${GREEN}========================================${NC}"
echo ""

# Check for PICO_SDK_PATH
if [ -z "$PICO_SDK_PATH" ]; then
    echo -e "${RED}Error: PICO_SDK_PATH not set${NC}"
    echo "Please set PICO_SDK_PATH to your Pico SDK installation"
    echo "Example: export PICO_SDK_PATH=/path/to/pico-sdk"
    exit 1
fi

echo -e "${YELLOW}Using Pico SDK: $PICO_SDK_PATH${NC}"
echo ""

# Check if build directory exists
if [ -d "build" ]; then
    echo -e "${YELLOW}Cleaning previous build...${NC}"
    rm -rf build
fi

# Create build directory
echo -e "${YELLOW}Creating build directory...${NC}"
mkdir build
cd build

# Run CMake
echo -e "${YELLOW}Running CMake...${NC}"
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build
echo -e "${YELLOW}Building firmware...${NC}"
make -j$(nproc)

# Check if build succeeded
if [ -f "hitchhikers_guide.uf2" ]; then
    echo ""
    echo -e "${GREEN}========================================${NC}"
    echo -e "${GREEN}Build successful!${NC}"
    echo -e "${GREEN}========================================${NC}"
    echo ""
    echo -e "Firmware file: ${GREEN}build/hitchhikers_guide.uf2${NC}"
    echo ""
    echo "To install:"
    echo "1. Hold BOOTSEL button on Pico while connecting USB"
    echo "2. Copy hitchhikers_guide.uf2 to the USB drive"
    echo "3. Device will reboot automatically"
    echo ""
    echo -e "${YELLOW}DON'T PANIC!${NC}"
    echo ""
else
    echo ""
    echo -e "${RED}Build failed!${NC}"
    echo "Check the output above for errors"
    exit 1
fi