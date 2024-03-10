#pragma once

#include <iostream>
#include <stddef.h>
#include <SDL.h>
#include "BLDefs.hpp"

class Node;
class TextureInfo;

struct RenderInfo
{
    // Position and Size
    Rect dest;

    int textureID; // Use an appropriate type for your texture identifiers

    // Rotation and Scale
    float rotation;

    // Layer/Z-Index
    int layerID;

    // Additional Rendering Flags or Modifiers
    bool visible;
    SDL_RendererFlip flip;
    //float opacity;

    SDL_Rect clip;
    SDL_Point center;
    bool useCenter = false;

    RenderInfo() : dest({0,0,0,0}), textureID(0), rotation(0),
                   layerID(0), visible(true), flip(SDL_FLIP_NONE), clip({0,0,0,0}), center({0,0}) {}
};

inline void printInfo(RenderInfo& info)
{
    std::cout << "tid:" << info.textureID << std::endl;
    std::cout << "rpt:" << info.rotation << std::endl;
    std::cout << "lid:" << info.layerID << std::endl;
    std::cout << "flip:" << info.flip << std::endl;
    std::cout << "vis:" << info.visible << std::endl;
}

RenderInfo createInfo(Node* node, TextureInfo* tinfo);
RenderInfo createInfo(const Node& node, const TextureInfo& tinfo);
