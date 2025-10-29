#!/usr/bin/env python3
from PIL import Image
import sys

def rle_compress(pixels):
    """Simple RLE compression for RGB data"""
    compressed = []
    i = 0
    while i < len(pixels):
        current = pixels[i]
        count = 1
        while i + count < len(pixels) and count < 255 and pixels[i + count] == current:
            count += 1
        compressed.append((count, current))
        i += count
    return compressed

if len(sys.argv) < 2:
    print("Usage: python3 compress_image.py input.jpg")
    sys.exit(1)

# Load and resize
img = Image.open(sys.argv[1])
img = img.resize((320, 320), Image.Resampling.LANCZOS)
img = img.convert('RGB')

# Get pixels
pixels = list(img.getdata())

# Compress
compressed = rle_compress(pixels)

# Generate C code
print("/* Compressed DON'T PANIC image */")
print("#include <stdint.h>")
print(f"// Original: {len(pixels)*3} bytes, Compressed: {len(compressed)*4} bytes")
print(f"const uint32_t dont_panic_compressed[] = {{")

for i, (count, (r, g, b)) in enumerate(compressed):
    # Pack: count(8bit) | R(8bit) | G(8bit) | B(8bit)
    packed = (count << 24) | (r << 16) | (g << 8) | b
    if i % 4 == 0:
        print("    ", end="")
    print(f"0x{packed:08X},", end="")
    if i % 4 == 3:
        print()

print("\n};")
print(f"const uint32_t dont_panic_size = {len(compressed)};")