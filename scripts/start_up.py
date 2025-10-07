import subprocess
import signal
import sys
import subprocess
import re

def generate_emscripten_profile(filename):
    try:
        # Run the 'clang --version' command and capture the output
        result = subprocess.run(['clang', '--version'], stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True, check=True)
        version_output = result.stdout

        # Determine if it's apple-clang or regular clang and extract the version
        if "Apple clang" in version_output:
            compiler = "apple-clang"
            version_match = re.search(r'Apple clang version (\d+(\.\d+)*)', version_output)
        else:
            compiler = "clang"
            version_match = re.search(r'clang version (\d+(\.\d+)*)', version_output)

        if not version_match:
            raise ValueError("Could not parse clang version from output")

        clang_version = version_match.group(1).split('.')[0]


        
    except Exception:
        # Default to Emscripten-provided clang version if system clang is not available
        compiler = "clang"
        clang_version = "6.0"

    # Generate the profile content
    profile_content = f"""[settings]
os=Emscripten
arch=wasm
compiler={compiler}
compiler.cppstd=gnu17
compiler.libcxx=libc++
compiler.version={clang_version}
build_type=Release
[tool_requires]
emsdk/3.1.50
"""

    # Write the profile content to the specified file
    with open(filename, 'w') as file:
        file.write(profile_content)

    print(f"Profile written to {filename}")


server_running = True

def start_emrun(port, output_html):
    try:
        # Call emrun to start the server without opening the browser
        emrun_process = subprocess.Popen(
            ['emrun', '--port', str(port), '.'],
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE
        )
        print(f"emrun started, serving at http://localhost:{port}/{output_html}")

        return emrun_process
    except FileNotFoundError:
        print("emrun not found. Ensure Emscripten is installed and environment is set up.")
        sys.exit(1)

# Function to handle termination signal and shut down the server
def signal_handler(emrun_process):
    global server_running
    if server_running:
        print("Shutting down the server...")
        emrun_process.terminate()
        emrun_process.wait()
        print("Shut down server")
        server_running = False

def start_up(port, output_html):
    # Start emrun server
    emrun_process = start_emrun(port, output_html)

    # Set up signal handling to catch termination signals
    signal.signal(signal.SIGINT, lambda s, f: signal_handler(emrun_process))
    signal.signal(signal.SIGTERM, lambda s, f: signal_handler(emrun_process))
    global server_running
    # Keep the main thread running until terminated
    try:
        while server_running:
            pass
    except (KeyboardInterrupt, SystemExit):
        print("\nCtrl+C received. The server will be stopped, but the script will keep running.")
        signal_handler(emrun_process)
