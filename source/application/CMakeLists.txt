# Include directories
target_include_directories(${PROJECT_NAME}
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

# Sources
target_sources(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/BLApplication.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/BlinkGame.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/BlinkMenu.cpp
)
