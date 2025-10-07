import subprocess
import os
import sys
import platform

class VenvContext:
    def __init__(self, name):
        self.name = name
        self.venv_path = os.path.abspath(name)
        self.bin_path = self._get_bin_path()
        self.venv_py = self._get_venv_py()

    def _get_bin_path(self):
        """Returns the path to the bin or Scripts directory based on the operating system."""
        if platform.system() == "Windows":
            return os.path.join(self.venv_path, 'Scripts')
        else:
            return os.path.join(self.venv_path, 'bin')

    def _get_venv_py(self):
        """Returns the path to the Python executable within the virtual environment."""
        if platform.system() == "Windows":
            return os.path.join(self.bin_path, 'python.exe')
        else:
            return os.path.join(self.bin_path, 'python')

    def run_in_venv(self, name, full_command, to_console=True):
        print(f"Running: {name}")
        return_value = None if to_console else subprocess.PIPE
        try:
            result = subprocess.run(
                full_command,
                text=True,  # Handle outputs as text (decode bytes to str)
                check=True,  # Raise exception on non-zero exit (which is caught below)
                capture_output=False,  # Don't capture output here
                stdout=return_value,
            )
            return result
        except subprocess.CalledProcessError as e:
            print(f"Failed command: {' '.join(full_command)}")
            sys.exit(1)

    def run_script(self, script_path, *args):
        """Runs a Python script located at script_path with additional arguments using the virtual environment's Python."""
        command = [self.venv_py, script_path] + list(args)
        return self.run_in_venv(os.path.basename(script_path), command)

    def run_command(self, command, *args, return_value=None):
        """Runs a command from the bin directory of the virtual environment."""
        executable = os.path.join(self.bin_path, command)
        full_command = [executable] + list(args)
        return self.run_in_venv(command, full_command, return_value)

def create_venv(venv_context: VenvContext):
    subprocess.check_call([sys.executable, '-m', 'venv', venv_context.name])

def initialize_venv(venv_context: VenvContext, requirements):
    venv_context.run_in_venv("Version", [venv_context.venv_py, '--version'])
    venv_context.run_in_venv("Upgrade pip", [venv_context.venv_py, '-m', 'pip', 'install', '--upgrade', 'pip'])
    venv_context.run_in_venv("Install requirements", [venv_context.venv_py, '-m', 'pip', 'install', '-r', requirements])

