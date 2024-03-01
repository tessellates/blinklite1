#pragma once

#include <iostream>
#include <stddef.h>
#include <SDL.h>

struct RenderInfo
{
    // Position and Size
    SDL_Rect dest;

    int textureID; // Use an appropriate type for your texture identifiers

    // Rotation and Scale
    float rotation;

    // Layer/Z-Index
    int layerID;

    // Additional Rendering Flags or Modifiers
    bool visible;
    SDL_RendererFlip flip;
    float opacity;

    SDL_Rect clip;

    RenderInfo() : dest({}), textureID(0), rotation(0),
                   layerID(0), visible(true), flip(SDL_FLIP_NONE), opacity(1.0f), clip({}) {}
};

inline void printInfo(RenderInfo& info)
{
    std::cout << "tid:" << info.textureID << std::endl;
    std::cout << "rpt:" << info.rotation << std::endl;
    std::cout << "lid:" << info.layerID << std::endl;
    std::cout << "flip:" << info.flip << std::endl;
    std::cout << "op:" << info.opacity << std::endl;
    std::cout << "vis:" << info.visible << std::endl;
}
