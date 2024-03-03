#pragma once

#include <SDL.h>

struct TextureInfo
{
    int textureID = 0;
    SDL_RendererFlip flip = SDL_FLIP_NONE;
    float angle = 0;
    SDL_Rect dest {0,0,0,0};
    SDL_Rect clip {0,0,0,0};
    int layerID;
};
