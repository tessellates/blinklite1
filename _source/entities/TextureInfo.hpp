#pragma once

#include <SDL3/SDL.h>
#include "BLutils.hpp"

struct TextureInfo
{
    int textureID = 0;
    SDL_FlipMode flip = SDL_FLIP_NONE;
    float angle = 0;
    Rect dest {0,0,0,0};
    SDL_FRect clip {0,0,0,0};
    int layerID = 0;
};
