# scripts/script_utils.py

from .virtual_env_tools import *  # or import specific functions/classes
from .env_tools import *  # or import specific functions/classes
from .start_up import *  # or import specific functions/classes
import shutil

def copy_files(src, dest):
    if not os.path.exists(src):
        print(f"Source directory {src} does not exist.")
        return
    if not os.path.exists(dest):
        os.makedirs(dest)
        print(f"Created destination directory {dest}")
    for filename in os.listdir(src):
        src_file = os.path.join(src, filename)
        dest_file = os.path.join(dest, filename)
        shutil.copy(src_file, dest_file)
        print(f"Copied {src_file} to {dest_file}")

def remove_folder(folder_path):
    if os.path.exists(folder_path):
        shutil.rmtree(folder_path)
        print(f"Removed folder: {folder_path}")
    else:
        print(f"Folder does not exist: {folder_path}")