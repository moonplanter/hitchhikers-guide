#!/usr/bin/env python3
"""
Hitchhiker's Guide Article Manager
Helps create, edit, and organize Guide articles
"""

import os
import sys
from pathlib import Path

GUIDE_DIR = "sd_card/guide"
INDEX_FILE = f"{GUIDE_DIR}/index.txt"

CATEGORIES = [
    "Planets",
    "Species",
    "Technology",
    "Beverages",
    "Essential Items",
    "Spacecraft",
    "Personalities",
    "Computers",
    "Philosophy",
    "Infrastructure",
    "Culture",
    "Locations",
    "Corporations",
    "Advertising",
    "Science"
]

def ensure_guide_dir():
    """Create guide directory if it doesn't exist"""
    Path(GUIDE_DIR).mkdir(parents=True, exist_ok=True)
    if not os.path.exists(INDEX_FILE):
        with open(INDEX_FILE, 'w') as f:
            f.write("")

def create_article():
    """Interactive article creation"""
    print("\n" + "="*50)
    print("CREATE NEW GUIDE ARTICLE")
    print("="*50)
    
    title = input("\nArticle title: ").strip()
    if not title:
        print("Title cannot be empty!")
        return
    
    # Generate filename
    filename = title.lower().replace(" ", "_").replace("'", "")
    filename = "".join(c for c in filename if c.isalnum() or c == "_")
    filename += ".txt"
    
    # Select category
    print("\nAvailable categories:")
    for i, cat in enumerate(CATEGORIES, 1):
        print(f"{i}. {cat}")
    
    while True:
        try:
            cat_num = int(input("\nSelect category (number): "))
            if 1 <= cat_num <= len(CATEGORIES):
                category = CATEGORIES[cat_num - 1]
                break
            print("Invalid category number!")
        except ValueError:
            print("Please enter a number!")
    
    # Get content
    print("\nEnter article content (type END on a new line to finish):")
    print("-" * 50)
    
    lines = []
    while True:
        line = input()
        if line == "END":
            break
        lines.append(line)
    
    content = "\n".join(lines)
    
    # Write article file
    article_path = f"{GUIDE_DIR}/{filename}"
    with open(article_path, 'w') as f:
        f.write(content)
    
    # Add to index
    with open(INDEX_FILE, 'a') as f:
        f.write(f"{title}|{filename}|{category}\n")
    
    print("\n✓ Article created successfully!")
    print(f"  File: {article_path}")
    print(f"  Title: {title}")
    print(f"  Category: {category}")

def list_articles():
    """List all articles with their categories"""
    if not os.path.exists(INDEX_FILE):
        print("No articles found!")
        return
    
    print("\n" + "="*50)
    print("GUIDE ARTICLES")
    print("="*50)
    
    with open(INDEX_FILE, 'r') as f:
        lines = f.readlines()
    
    if not lines:
        print("No articles found!")
        return
    
    # Group by category
    by_category = {}
    for line in lines:
        if not line.strip():
            continue
        parts = line.strip().split('|')
        if len(parts) != 3:
            continue
        title, filename, category = parts
        if category not in by_category:
            by_category[category] = []
        by_category[category].append((title, filename))
    
    # Display
    for category in sorted(by_category.keys()):
        print(f"\n{category}:")
        for title, filename in sorted(by_category[category]):
            print(f"  • {title} ({filename})")
    
    print(f"\nTotal articles: {len(lines)}")

def search_articles(query):
    """Search articles by title"""
    if not os.path.exists(INDEX_FILE):
        print("No articles found!")
        return
    
    print(f"\nSearching for: {query}")
    print("="*50)
    
    found = []
    with open(INDEX_FILE, 'r') as f:
        for line in f:
            if not line.strip():
                continue
            parts = line.strip().split('|')
            if len(parts) != 3:
                continue
            title, filename, category = parts
            if query.lower() in title.lower():
                found.append((title, filename, category))
    
    if found:
        for title, filename, category in found:
            print(f"• {title}")
            print(f"  File: {filename}")
            print(f"  Category: {category}")
            print()
    else:
        print("No matches found!")

def view_article(query):
    """View an article's content"""
    if not os.path.exists(INDEX_FILE):
        print("No articles found!")
        return
    
    # Find article
    with open(INDEX_FILE, 'r') as f:
        for line in f:
            if not line.strip():
                continue
            parts = line.strip().split('|')
            if len(parts) != 3:
                continue
            title, filename, category = parts
            if query.lower() in title.lower():
                article_path = f"{GUIDE_DIR}/{filename}"
                if os.path.exists(article_path):
                    print("\n" + "="*50)
                    print(title.upper())
                    print("="*50)
                    with open(article_path, 'r') as af:
                        print(af.read())
                    print("\n" + "="*50)
                    print(f"Category: {category}")
                    return
    
    print(f"Article not found: {query}")

def validate_articles():
    """Check for missing files and index issues"""
    print("\n" + "="*50)
    print("VALIDATING GUIDE DATABASE")
    print("="*50)
    
    if not os.path.exists(INDEX_FILE):
        print("✗ Index file not found!")
        return
    
    issues = []
    article_count = 0
    
    with open(INDEX_FILE, 'r') as f:
        for i, line in enumerate(f, 1):
            if not line.strip():
                continue
            
            parts = line.strip().split('|')
            if len(parts) != 3:
                issues.append(f"Line {i}: Invalid format")
                continue
            
            title, filename, category = parts
            article_count += 1
            
            # Check file exists
            article_path = f"{GUIDE_DIR}/{filename}"
            if not os.path.exists(article_path):
                issues.append(f"Line {i}: Missing file '{filename}' for '{title}'")
            
            # Check category
            if category not in CATEGORIES:
                issues.append(f"Line {i}: Unknown category '{category}' for '{title}'")
    
    # Check for orphaned files
    if os.path.exists(GUIDE_DIR):
        index_files = set()
        with open(INDEX_FILE, 'r') as f:
            for line in f:
                if line.strip():
                    parts = line.strip().split('|')
                    if len(parts) == 3:
                        index_files.add(parts[1])
        
        for filename in os.listdir(GUIDE_DIR):
            if filename.endswith('.txt') and filename != 'index.txt':
                if filename not in index_files:
                    issues.append(f"Orphaned file: {filename} (not in index)")
    
    # Report
    print(f"\nArticles in index: {article_count}")
    
    if issues:
        print(f"\n✗ Found {len(issues)} issue(s):")
        for issue in issues:
            print(f"  • {issue}")
    else:
        print("\n✓ All checks passed!")

def export_to_sd():
    """Instructions for copying to SD card"""
    print("\n" + "="*50)
    print("EXPORT TO SD CARD")
    print("="*50)
    print("\n1. Format SD card as FAT32")
    print("2. Create folder 'guide' on the SD card")
    print(f"3. Copy all files from '{GUIDE_DIR}/' to SD card's 'guide/' folder")
    print("4. Insert SD card into PicoCalc")
    print("\nSD card structure should be:")
    print("  SD_CARD/")
    print("  └── guide/")
    print("      ├── index.txt")
    print("      ├── earth.txt")
    print("      ├── towel.txt")
    print("      └── [more articles...]")

def show_stats():
    """Show statistics about the Guide"""
    if not os.path.exists(INDEX_FILE):
        print("No articles found!")
        return
    
    print("\n" + "="*50)
    print("GUIDE STATISTICS")
    print("="*50)
    
    by_category = {}
    total = 0
    total_words = 0
    
    with open(INDEX_FILE, 'r') as f:
        for line in f:
            if not line.strip():
                continue
            parts = line.strip().split('|')
            if len(parts) != 3:
                continue
            
            title, filename, category = parts
            total += 1
            
            # Count words
            article_path = f"{GUIDE_DIR}/{filename}"
            if os.path.exists(article_path):
                with open(article_path, 'r') as af:
                    content = af.read()
                    total_words += len(content.split())
            
            by_category[category] = by_category.get(category, 0) + 1
    
    print(f"\nTotal articles: {total}")
    print(f"Total words: {total_words:,}")
    print(f"Average words per article: {total_words // total if total > 0 else 0}")
    
    print("\nArticles by category:")
    for category in sorted(by_category.keys()):
        count = by_category[category]
        print(f"  {category}: {count}")

def main():
    ensure_guide_dir()
    
    if len(sys.argv) < 2:
        print("\nHitchhiker's Guide Article Manager")
        print("="*50)
        print("\nCommands:")
        print("  create       - Create a new article")
        print("  list         - List all articles")
        print("  search TERM  - Search for articles")
        print("  view TITLE   - View an article")
        print("  validate     - Check for issues")
        print("  stats        - Show statistics")
        print("  export       - Export instructions")
        print("\nExample:")
        print("  python3 article_manager.py create")
        print("  python3 article_manager.py search earth")
        print("  python3 article_manager.py view towel")
        return
    
    command = sys.argv[1].lower()
    
    if command == "create":
        create_article()
    elif command == "list":
        list_articles()
    elif command == "search":
        if len(sys.argv) < 3:
            print("Usage: article_manager.py search TERM")
        else:
            search_articles(" ".join(sys.argv[2:]))
    elif command == "view":
        if len(sys.argv) < 3:
            print("Usage: article_manager.py view TITLE")
        else:
            view_article(" ".join(sys.argv[2:]))
    elif command == "validate":
        validate_articles()
    elif command == "stats":
        show_stats()
    elif command == "export":
        export_to_sd()
    else:
        print(f"Unknown command: {command}")
        print("Run without arguments to see available commands")

if __name__ == "__main__":
    main()