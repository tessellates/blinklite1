cmake_minimum_required(VERSION 3.10)
project(textureCheck)

# Specify the required C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)


# Define the executable for this example
add_executable(textureCheck main.cpp)

add_subdirectory(source)

# Link the library
target_link_libraries(textureCheck ${BLINKLIB})

add_compile_definitions(IMGUI_DEFINE_MATH_OPERATORS)

# If the example has special dependencies, find or add them here
# find_package(SomeDependency)
# target_link_libraries(Example1 SomeDependency)
