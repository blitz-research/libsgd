import os
import re
import tkinter as tk
from tkinter import filedialog, messagebox, simpledialog

def replace_identifiers_with_prefix(file_path, replacements):
    with open(file_path, 'r', newline='') as file:
        content = file.read()

    # Define the pattern to match identifiers with sgd_ or SGD_ prefix
    pattern = re.compile(r'\b(sgd_|SGD_)(\w+)\b')

    def replace_match(match):
        prefix = match.group(1)
        identifier = match.group(2)
        if identifier in replacements:
            return prefix + replacements[identifier]
        return match.group(0)  # No replacement, return original match

    # Apply the replacement to the content
    content = pattern.sub(replace_match, content)

    with open(file_path, 'w', newline='') as file:
        file.write(content)

def replace_identifiers_simple(file_path, replacements):
    with open(file_path, 'r', newline='') as file:
        content = file.read()

    for old_id, new_id in replacements.items():
        # Use regular expression to match whole words only
        content = re.sub(rf'\b{re.escape(old_id)}\b', new_id, content)

    with open(file_path, 'w', newline='') as file:
        file.write(content)

def process_directory(root_dir, replacements, file_types):
    for dirpath, dirnames, filenames in os.walk(root_dir):
        for filename in filenames:
            file_path = os.path.join(dirpath, filename)
            if any(filename.endswith(ext) for ext in file_types):
                if filename.endswith(('.c', '.cpp')):
                    replace_identifiers_with_prefix(file_path, replacements)
                else:
                    replace_identifiers_simple(file_path, replacements)

def main():
    # Initialize tkinter and hide main window
    root = tk.Tk()
    root.withdraw()

    # Prompt the user to select the root directory
    root_dir = filedialog.askdirectory(title="Select Root Directory")
    if not root_dir:
        print("No directory selected. Exiting.")
        return

    # Prompt the user to enter file extensions (e.g., .txt, .cpp, .py)
    file_types_input = simpledialog.askstring(
        "File Types",
        "Enter file extensions to process (comma-separated, e.g., .txt, .cpp, .py):"
    )
    if not file_types_input:
        print("No file types specified. Exiting.")
        return

    # Process the input and split it into a list of extensions
    file_types = [ext.strip() for ext in file_types_input.split(',')]

    # Show a warning dialog
    proceed = messagebox.askyesno(
        "Warning",
        f"You are about to process all files with the following extensions in:\n\n{root_dir}\n\n{', '.join(file_types)}\n\nThis action will replace identifiers in all matching files. Do you want to continue?"
    )
    if not proceed:
        print("Operation canceled by the user.")
        return

    # Example replacement dictionary (without prefix)
    replacement_dict = {
        'GetEntityEnabled': 'IsEntityEnabled',
        'GetEntityVisible': 'IsEntityVisible',
    }

    process_directory(root_dir, replacement_dict, file_types)
    print("Replacement process completed.")

if __name__ == "__main__":
    main()
