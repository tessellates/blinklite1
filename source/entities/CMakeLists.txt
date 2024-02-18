# Include directories
target_include_directories(${PROJECT_NAME}
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

# Sources
target_sources(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Node.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/MovementComponent.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/AccMovementComponent.cpp
)
