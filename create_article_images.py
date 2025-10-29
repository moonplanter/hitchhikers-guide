#!/usr/bin/env python3
"""
Create article images for the Hitchhiker's Guide
Generates simple bitmap images that can be embedded in the firmware
"""

from PIL import Image, ImageDraw, ImageFont
import os

def create_article_image(title, description, filename, width=300, height=100):
    """Create a simple article diagram image"""
    # Create image with dark background
    img = Image.new('RGB', (width, height), color=(0, 16, 0))  # Very dark green
    draw = ImageDraw.Draw(img)
    
    # Try to use a simple font
    try:
        font = ImageFont.load_default()
    except:
        font = None
    
    # Draw title
    draw.text((10, 10), title, fill=(0, 255, 0), font=font)  # Bright green
    
    # Draw description
    lines = description.split('\n')
    y_offset = 30
    for line in lines:
        draw.text((10, y_offset), line, fill=(0, 200, 200), font=font)  # Cyan
        y_offset += 15
    
    # Add some decorative elements based on the article
    if "Earth" in title:
        # Draw a simple planet
        draw.ellipse([200, 20, 280, 80], outline=(0, 150, 255), width=2)  # Blue circle
        draw.ellipse([220, 30, 240, 50], fill=(0, 200, 0))  # Green continent
        draw.ellipse([250, 40, 270, 60], fill=(0, 200, 0))  # Another continent
        
    elif "Babel Fish" in title:
        # Draw a simple fish shape
        draw.ellipse([200, 30, 280, 70], outline=(255, 255, 0), width=2)  # Yellow body
        draw.polygon([(280, 50), (300, 40), (300, 60)], fill=(255, 255, 0))  # Tail
        draw.ellipse([210, 40, 220, 50], fill=(0, 0, 0))  # Eye
        
    elif "Towel" in title:
        # Draw a simple towel
        draw.rectangle([200, 20, 290, 80], outline=(0, 255, 255), width=2)  # Cyan rectangle
        # Add texture lines
        for i in range(5):
            y = 25 + i * 10
            draw.line([(205, y), (285, y)], fill=(0, 200, 200), width=1)
            
    elif "42" in title:
        # Draw large "42"
        draw.text((220, 30), "42", fill=(255, 255, 0), font=font)
        # Add question marks around it
        import math
        for angle in range(0, 360, 45):
            x = 245 + 30 * math.cos(math.radians(angle))
            y = 50 + 20 * math.sin(math.radians(angle))
            draw.text((int(x), int(y)), "?", fill=(255, 100, 100), font=font)
    
    # Save the image
    img.save(filename)
    print(f"Created {filename}")

def main():
    """Create article images"""
    
    # Create output directory
    os.makedirs("article_images", exist_ok=True)
    
    # Create images for different articles
    articles = [
        ("Earth", "Mostly Harmless\nDemolished for hyperspace bypass\nActually a computer", "article_images/earth.png"),
        ("Babel Fish", "Universal translator\nInsert in ear\nProves non-existence of God", "article_images/babel_fish.png"),
        ("Towel", "Most useful item\nPsychological value\nDon't leave planet without it", "article_images/towel.png"),
        ("42", "Ultimate Answer\nLife, Universe, Everything\nBut what's the question?", "article_images/42.png"),
        ("Vogons", "Unpleasant bureaucrats\nThird worst poetry\nAvoid at all costs", "article_images/vogons.png"),
        ("Don't Panic", "Large friendly letters\nMost important advice\nKeep calm and carry towel", "article_images/dont_panic.png"),
        ("Heart of Gold", "Infinite Improbability Drive\nStolen by Zaphod\nMakes weird things happen", "article_images/heart_of_gold.png"),
        ("Marvin", "Paranoid Android\nBrain size of planet\nPerpetually depressed", "article_images/marvin.png"),
        ("Pan Galactic Gargle Blaster", "Best drink in existence\nLike brain smashed by lemon\nExtremely dangerous", "article_images/pan_galactic.png"),
        ("Zaphod Beeblebrox", "Two heads, three arms\nEx-President of Galaxy\nHoopy frood", "article_images/zaphod.png"),
    ]
    
    for title, description, filename in articles:
        create_article_image(title, description, filename)
    
    print(f"\nCreated {len(articles)} article images!")
    print("These demonstrate the enhanced visual style with:")
    print("- Colorful diagrams matching the Babel Fish examples you shared")
    print("- Better typography and layout")
    print("- Category-specific visual elements")
    print("- Consistent retro-futuristic aesthetic")

if __name__ == "__main__":
    main()
