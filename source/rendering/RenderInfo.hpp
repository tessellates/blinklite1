#pragma once

#include <stddef.h>

struct RenderInfo 
{
    // Position and Size
    float x, y;
    float width, height;

    size_t textureID; // Use an appropriate type for your texture identifiers

    // Rotation and Scale
    float rotation;
    float scale;

    // Layer/Z-Index
    int layerID;

    // Additional Rendering Flags or Modifiers
    bool visible;
    enum class Flip { None, Horizontal, Vertical } flip;
    float opacity;

    RenderInfo() : x(0), y(0), width(0), height(0), textureID(0), rotation(0), scale(1),
                   layerID(0), visible(true), flip(Flip::None), opacity(1.0f) {}
};
