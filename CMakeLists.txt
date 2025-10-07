cmake_minimum_required(VERSION 3.14)

#LANGUAGES can be: C , CXX (i.e. C++)
project(blinklite VERSION 1.0
DESCRIPTION "2D engine -- for simple 2D games"
LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 20)  # Set the C++ standard to C++17
set(CMAKE_CXX_STANDARD_REQUIRED ON)  # Make the C++ standard required

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${${PROJECT_NAME}_BINARY_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${${PROJECT_NAME}_BINARY_DIR}/bin")

if(WIN32)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
else()
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}")
endif()

set(CMAKE_PREFIX_PATH "${CMAKE_PREFIX_PATH};${CMAKE_CURRENT_SOURCE_DIR}/build")

# To prevent Apple frameworks from being included before brew installed packages
include_directories(BEFORE /usr/local/include)
set(CMAKE_FIND_FRAMEWORK LAST) 

# SDL
find_package(SDL3 REQUIRED CONFIG)
find_package(SDL3_ttf REQUIRED CONFIG)
find_package(glm REQUIRED CONFIG)

# Create the library
add_library(${PROJECT_NAME})

# submodules
add_subdirectory(extern) 

# source -- source for the library
add_subdirectory(source) 

# staging -- these are test executables to run
option(BUILD_STAGING "Build staging" ON)
if(BUILD_STAGING)
    add_subdirectory(staging) 
endif()

set(BLINKLIB ${PROJECT_NAME})

# examples -- fully fletched out examples
option(BUILD_EXAMPLES "Build examples" OFF)
if(BUILD_EXAMPLES)
    #add_subdirectory(examples) 
endif()


# testing
option(BUILD_TESTS "Build tests" OFF)

if(BUILD_TESTS)
    #googletest
    find_package(GTest CONFIG REQUIRED) 
    # Enable testing for the project.
    enable_testing()
    # Include your tests subdirectory.
    add_subdirectory(tests)
endif()

# Link SDL to the library
target_link_libraries(${PROJECT_NAME} PUBLIC glm::glm)
target_link_libraries(${PROJECT_NAME} PUBLIC SDL3::SDL3)
#target_link_libraries(${PROJECT_NAME} PUBLIC SDL3_ttf::SDL3_ttf-static)
# Enable imgui default math operator
add_compile_definitions(IMGUI_DEFINE_MATH_OPERATORS)

# Copy assets to the build directory
file(COPY assets/ DESTINATION ${CMAKE_BINARY_DIR}/assets)