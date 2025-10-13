# Include directories
target_include_directories(${PROJECT_NAME}
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

# Sources
target_sources(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Engine.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/BaseModule.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/BlinkMenu.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/GameClock.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/EventStack.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/FrameRateCounter.cpp
)
