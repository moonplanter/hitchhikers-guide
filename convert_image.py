#!/usr/bin/env python3
from PIL import Image
import sys

if len(sys.argv) < 2:
    print("Usage: python3 convert_image.py input.jpg")
    sys.exit(1)

img = Image.open(sys.argv[1])
img = img.resize((320, 320), Image.Resampling.LANCZOS)
img = img.convert('RGB')

with open('dont_panic.raw', 'wb') as f:
    for pixel in img.getdata():
        f.write(bytes(pixel))

print("Created dont_panic.raw")
print("Copy to SD:/guide/dont_panic.raw")