#!/usr/bin/python3
"""
BlinkLite Build Script

A professional build automation script for the BlinkLite project.
Supports native builds, Emscripten WebAssembly compilation, and application execution.

Author: BlinkLite Team
License: See project LICENSE file
"""

from typing import Optional
import argparse
import logging
import os
import platform
import subprocess
import sys

from scripts import script_utils

# Constants
DEFAULT_BUILD_TYPE = 'Debug'
VENV_NAME = 'blink_lite_venv'
BUILD_DIR = 'build'
DATA_DIR = 'data'
REQUIREMENTS_FILE = os.path.join(DATA_DIR, 'requirements.txt')
EMSCRIPTEN_PROFILE_PATH = os.path.join(DATA_DIR, 'emprofile')
DEFAULT_CONAN_PROFILE = 'default'

# Configure logging
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(levelname)s - %(message)s'
)
logger = logging.getLogger(__name__)


class BuildError(Exception):
    """Custom exception for build-related errors."""
    pass


def setup_environment() -> script_utils.VenvContext:
    """
    Set up the build environment and return the virtual environment context.
    
    Returns:
        VenvContext: Configured virtual environment context
        
    Raises:
        BuildError: If environment setup fails
    """
    try:
        venv_context = script_utils.VenvContext(VENV_NAME)
        subprocess.check_call(['git', 'submodule', 'update', '--init', '--recursive'])
        os.environ['REPO_DIR'] = os.getcwd()
        logger.info("Environment setup completed successfully")
        return venv_context
    except Exception as e:
        raise BuildError(f"Failed to setup environment: {e}") from e


def setup_virtual_environment(venv_context: script_utils.VenvContext) -> None:
    """
    Create and initialize the virtual environment with dependencies.
    
    Args:
        venv_context: The virtual environment context
        
    Raises:
        BuildError: If virtual environment setup fails
    """
    try:
        logger.info("Creating virtual environment...")
        script_utils.create_venv(venv_context)
        
        logger.info("Installing Python dependencies...")
        script_utils.initialize_venv(venv_context, REQUIREMENTS_FILE)
        
        logger.info("Virtual environment setup completed")
    except Exception as e:
        raise BuildError(f"Failed to setup virtual environment: {e}") from e


def configure_conan_profiles(emscripten: bool = False) -> tuple[str, str]:
    """
    Configure and return the appropriate Conan profiles for the build.
    
    Args:
        emscripten: Whether to use Emscripten for WebAssembly compilation
        
    Returns:
        tuple: (build_profile, host_profile) pair
        
    Raises:
        BuildError: If profile configuration fails
    """
    try:
        build_profile = DEFAULT_CONAN_PROFILE
        host_profile = build_profile
        
        if emscripten:
            logger.info("Configuring Emscripten profile...")
            if not os.path.exists(EMSCRIPTEN_PROFILE_PATH):
                script_utils.generate_emscripten_profile(EMSCRIPTEN_PROFILE_PATH)
                logger.info(f"Generated Emscripten profile at {EMSCRIPTEN_PROFILE_PATH}")
            host_profile = EMSCRIPTEN_PROFILE_PATH
        
        return build_profile, host_profile
    except Exception as e:
        raise BuildError(f"Failed to configure Conan profiles: {e}") from e


def install_dependencies(
    venv_context: script_utils.VenvContext, 
    build_profile: str, 
    host_profile: str, 
    emscripten: bool = False
) -> None:
    """
    Install project dependencies using Conan.
    
    Args:
        venv_context: The virtual environment context
        build_profile: Conan build profile to use
        host_profile: Conan host profile to use  
        emscripten: Whether to enable Emscripten options
        
    Raises:
        BuildError: If dependency installation fails
    """
    try:
        logger.info("Detecting Conan profile...")
        try:
            venv_context.run_command('conan', 'profile', 'detect')
        except: 
            pass
        logger.info("Installing dependencies with Conan...")
        command_args = [
            'install', '.', 
            f'-of={BUILD_DIR}', 
            '-s', f'build_type={DEFAULT_BUILD_TYPE}', 
            '--build=missing',
            f'-pr:b={build_profile}','-pr:b=scripts/append_cmake_to_profile.txt', 
            f'-pr:h={host_profile}', '-pr:h=scripts/append_cmake_to_profile.txt'
        ]
        
        if emscripten:
            command_args.extend(['-o', 'emscripten=True'])
            
        venv_context.run_command('conan', *command_args)
        logger.info("Dependencies installed successfully")
        
    except subprocess.CalledProcessError as e:
        raise BuildError(f"Failed to install dependencies: {e}") from e


def build_project(
    venv_context: script_utils.VenvContext, 
    build_profile: str, 
    host_profile: str
) -> None:
    """
    Build the project using Conan.
    
    Args:
        venv_context: The virtual environment context
        build_profile: Conan build profile to use
        host_profile: Conan host profile to use
        
    Raises:
        BuildError: If the build fails
    """
    try:
        logger.info("Building project...")
        venv_context.run_command(
            'conan', 'build', '.', 
            f'-of={BUILD_DIR}', 
            '-s', f'build_type={DEFAULT_BUILD_TYPE}',
            f'-pr:b={build_profile}','-pr:b=scripts/append_cmake_to_profile.txt', 
            f'-pr:h={host_profile}', '-pr:h=scripts/append_cmake_to_profile.txt'
        )
        logger.info("Project built successfully")
        
    except subprocess.CalledProcessError as e:
        raise BuildError(f"Failed to build project: {e}") from e


def run_application() -> None:
    """
    Run the built application.
    
    Raises:
        BuildError: If the application fails to run
    """
    try:
        logger.info("Running application...")
        
        if platform.system() == 'Windows':
            executable_path = os.path.join(BUILD_DIR, 'bin', DEFAULT_BUILD_TYPE, 'main.exe')
        else:
            executable_path = os.path.join('.', BUILD_DIR, 'bin', 'main')
            
        if not os.path.exists(executable_path):
            raise BuildError(f"Executable not found: {executable_path}")
            
        subprocess.check_call([executable_path])
        logger.info("Application executed successfully")
        
    except subprocess.CalledProcessError as e:
        raise BuildError(f"Failed to run application: {e}") from e
    except FileNotFoundError as e:
        raise BuildError(f"Application executable not found: {e}") from e


def main(setup: bool = True, emscripten: bool = False, run_only: bool = False) -> None:
    """
    Main build function that orchestrates the entire build process.
    
    Args:
        setup: Whether to perform initial setup (venv, dependencies)
        emscripten: Whether to use Emscripten for WebAssembly compilation
        run_only: Whether to skip building and only run the application
        
    Raises:
        BuildError: If any step of the build process fails
    """
    try:
        logger.info("Starting BlinkLite build process...")
        
        venv_context = setup_environment()
        
        if not run_only:
            if setup:
                setup_virtual_environment(venv_context)
            
            build_profile, host_profile = configure_conan_profiles(emscripten)
            
            if setup:
                install_dependencies(venv_context, build_profile, host_profile, emscripten)
            
            build_project(venv_context, build_profile, host_profile)
        run_application()
            
        logger.info("Build process completed successfully")
        
    except BuildError as e:
        logger.error(f"Build failed: {e}")
        sys.exit(1)
    except Exception as e:
        logger.error(f"Unexpected error during build: {e}")
        sys.exit(1)
    
def parse_arguments() -> argparse.Namespace:
    """
    Parse and validate command line arguments.
    
    Returns:
        argparse.Namespace: Parsed command line arguments
    """
    parser = argparse.ArgumentParser(
        description="BlinkLite Build Script - Build automation for native and WebAssembly targets",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  python build.py                    # Full build with setup
  python build.py --no-setup        # Build without recreating venv
  python build.py --emscripten       # Build for WebAssembly
  python build.py --run-only         # Skip build, just run the app
  python build.py --clean-build      # Clean build directory first
        """.strip()
    )
    
    parser.add_argument(
        '-ns', '--no-setup', 
        action='store_true',
        help='Skip virtual environment and dependency setup'
    )
    
    parser.add_argument(
        '-em', '--emscripten', 
        action='store_true',
        help='Enable Emscripten compilation for WebAssembly target (generates .wasm, .js, .html)'
    )
    
    parser.add_argument(
        '-ro', '--run-only', 
        action='store_true',
        help='Skip build process and only run the compiled application'
    )
    
    parser.add_argument(
        '-cb', '--clean-build', 
        action='store_true',
        help='Remove build directory before starting (forces clean build)'
    )
    
    parser.add_argument(
        '-v', '--verbose', 
        action='store_true',
        help='Enable verbose logging output'
    )
    
    return parser.parse_args()


def clean_build_directory() -> None:
    """
    Remove the build directory to ensure a clean build.
    
    Raises:
        BuildError: If cleaning the build directory fails
    """
    try:
        if os.path.exists(BUILD_DIR):
            logger.info(f"Cleaning build directory: {BUILD_DIR}")
            script_utils.remove_folder(BUILD_DIR)
            logger.info("Build directory cleaned successfully")
        else:
            logger.info("Build directory doesn't exist, nothing to clean")
    except Exception as e:
        raise BuildError(f"Failed to clean build directory: {e}") from e


if __name__ == "__main__":
    args = parse_arguments()
    
    # Configure logging level
    if args.verbose:
        logging.getLogger().setLevel(logging.DEBUG)
        logger.debug("Verbose logging enabled")
    
    # Clean build directory if requested
    if args.clean_build:
        clean_build_directory()
    
    # Validate argument combinations
    if args.run_only and args.emscripten:
        logger.warning("--emscripten flag ignored when using --run-only")
    
    if args.run_only and args.no_setup:
        logger.warning("--no-setup flag ignored when using --run-only")
    
    # Run the main build process
    main(
        setup=not args.no_setup,
        emscripten=args.emscripten,
        run_only=args.run_only
    )
