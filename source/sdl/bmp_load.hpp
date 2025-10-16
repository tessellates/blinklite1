#pragma once

#include <SDL3/SDL.h>

inline SDL_Texture* LoadBMPTexture(SDL_Renderer* r, const char* path){
    SDL_Surface* s = SDL_LoadBMP(path);                     // 8/24/32bpp BMP
    if(!s){ SDL_Log("SDL_LoadBMP: %s", SDL_GetError()); return nullptr; }

    SDL_Surface* s32 = SDL_ConvertSurface(s, SDL_PIXELFORMAT_RGBA32);
    SDL_DestroySurface(s);
    if(!s32){ SDL_Log("Convert RGBA32: %s", SDL_GetError()); return nullptr; }

    SDL_Texture* t = SDL_CreateTextureFromSurface(r, s32);
    SDL_DestroySurface(s32);
    if(!t){ SDL_Log("CreateTexture: %s", SDL_GetError()); return nullptr; }

    SDL_SetTextureBlendMode(t, SDL_BLENDMODE_BLEND);
    return t; // caller: SDL_DestroyTexture(t)
}

inline SDL_Texture* LoadBMPTextureWithKey(SDL_Renderer* r, const char* path, SDL_Color key){
    SDL_Surface* s = SDL_LoadBMP(path);
    if(!s) return nullptr;
    Uint32 k = SDL_MapRGB(s->format, NULL, key.r, key.g, key.b);
    SDL_SetColorKey(s, SDL_TRUE, k);                        // 1-bit transparency
    SDL_Surface* s32 = SDL_ConvertSurfaceFormat(s, SDL_PIXELFORMAT_RGBA32);
    SDL_DestroySurface(s);
    SDL_Texture* t = s32 ? SDL_CreateTextureFromSurface(r, s32) : nullptr;
    if(s32) SDL_DestroySurface(s32);
    return t;
}