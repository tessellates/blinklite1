# Include directories
target_include_directories(${PROJECT_NAME}
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

# Sources
target_sources(${PROJECT_NAME}
    PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}/BLSDLRenderer.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/BLSDLRenderLayer.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/BLSDLTextureManager.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/FrameRateCounter.cpp
)
