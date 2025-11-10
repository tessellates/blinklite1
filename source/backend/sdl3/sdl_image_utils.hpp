#pragma once

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include "sdl_config.hpp"

inline SDL_Texture* LoadPNGTexture(SDL_Renderer* r, const char* path) {
    SDL_Surface* s = IMG_Load(path);
    if (!s) { 
        SDL_Log("IMG_Load: %s", IMG_GetError()); 
        return nullptr; 
    }

    // Convert to our standard pixel format
    SDL_Surface* s_converted = SDL_ConvertSurface(s, STANDARD_PIXEL_FORMAT);
    SDL_DestroySurface(s);
    if (!s_converted) { 
        SDL_Log("Convert to standard format: %s", SDL_GetError()); 
        return nullptr; 
    }

    SDL_Texture* t = SDL_CreateTextureFromSurface(r, s_converted);
    SDL_DestroySurface(s_converted);
    if (!t) { 
        SDL_Log("CreateTexture: %s", SDL_GetError()); 
        return nullptr; 
    }

    SDL_SetTextureBlendMode(t, SDL_BLENDMODE_BLEND);
    return t; // caller: SDL_DestroyTexture(t)
}

// Initialize SDL_image (call once at startup)
inline bool InitImageLoader() {
    int flags = IMG_INIT_PNG | IMG_INIT_JPG | IMG_INIT_WEBP;
    int result = IMG_Init(flags);
    if ((result & flags) != flags) {
        SDL_Log("IMG_Init failed: %s", IMG_GetError());
        return false;
    }
    return true;
}

// Cleanup SDL_image (call at shutdown)
inline void QuitImageLoader() {
    IMG_Quit();
}