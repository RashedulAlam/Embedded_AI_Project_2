import os
import sys
import subprocess
import shutil


def execute_shell_command(command):
    """Execute a shell command."""
    try:
        output = subprocess.check_output(
            command, shell=True, stderr=subprocess.STDOUT, universal_newlines=True)
        print(output)
    except subprocess.CalledProcessError as e:
        print("Error:", e.output)


def copy_to_src(file_path, dir_name):
    file_path = f'{file_path.split('.')[0]}.h'

    destination_directory = f'src/{dir_name}'
    os.makedirs(destination_directory, exist_ok=True)

    destination_file = os.path.join(
        destination_directory, os.path.basename(file_path))

    if os.path.exists(destination_file):
        print('exits')
        os.remove(destination_file)
        print(f"File '{destination_file}' already exists. Overriding...")

    try:
        shutil.move(file_path, destination_directory)
        print(f"File '{file_path}' copied to '{destination_directory}'.")

    except Exception as e:
        print("Error:", e)


def convert_files(directory):
    """List all files from the given directory."""
    if not os.path.isdir(directory):
        print(f"Error: '{directory}' is not a directory.")
        return

    for dir_name in os.listdir(directory):
        dir_path = os.path.join(directory, dir_name)
        if os.path.isdir(dir_path):
            for file_name in os.listdir(dir_path):
                file_path = os.path.join(directory, dir_name, file_name)
                if os.path.isfile(file_path):
                    command = f'python image-converter/convert_image.py "{
                        file_path}"'
                    execute_shell_command(command)
                    copy_to_src(file_path, dir_name)


if __name__ == "__main__":

    if len(sys.argv) != 2:
        print("Usage: python script.py <directory_path>")
        sys.exit(1)

    directory = sys.argv[1]

    convert_files(directory)
