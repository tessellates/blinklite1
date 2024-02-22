import xcpp_build_tools.xcpp_build_tools.cppxbuild as xcbt
import argparse

def parse_arguments():
    parser = argparse.ArgumentParser(description="Build system for C++ projects using CMake.")
    parser.add_argument('commands', nargs='*', help="Commands, targets to build, or actions to apply. Use 'all' to build all targets.")
    return parser.parse_args()

if __name__ == "__main__":
    args = parse_arguments()
    xbuild = xcbt.XBuild()
    xbuild.handle_arguments(args.commands)

