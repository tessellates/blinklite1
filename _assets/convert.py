#!/usr/bin/env python3
import os
import sys
from PIL import Image

def convert_png_to_bmp(src_path, dst_path=None):
    """Convert one PNG file to BMP."""
    if not os.path.isfile(src_path):
        print(f"❌ File not found: {src_path}")
        return

    if not src_path.lower().endswith(".png"):
        print(f"⚠️ Skipping non-PNG file: {src_path}")
        return

    # Default output name if not specified
    if dst_path is None:
        dst_path = os.path.splitext(src_path)[0] + ".bmp"

    try:
        img = Image.open(src_path).convert("RGBA")
        img.save(dst_path, "BMP")
        print(f"✅ Converted: {src_path} → {dst_path}")
    except Exception as e:
        print(f"❌ Failed to convert {src_path}: {e}")

def batch_convert(folder):
    """Convert all PNG files in a folder (recursively)."""
    for root, _, files in os.walk(folder):
        for name in files:
            if name.lower().endswith(".png"):
                src = os.path.join(root, name)
                dst = os.path.splitext(src)[0] + ".bmp"
                convert_png_to_bmp(src, dst)

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage:")
        print("  python png_to_bmp.py file.png")
        print("  python png_to_bmp.py folder/")
        sys.exit(1)

    target = sys.argv[1]

    if os.path.isdir(target):
        batch_convert(target)
    else:
        convert_png_to_bmp(target)