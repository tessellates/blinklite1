#pragma once

#include <SDL.h>
#include <SDL_image.h>
#include <vector>

inline SDL_Texture* CreateColorTexture(SDL_Renderer* renderer, Uint32 color) {
    // Create a texture that will be used as a render target
    SDL_Texture* colorTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 2, 2);

    // Set the texture as the current rendering target
    SDL_SetRenderTarget(renderer, colorTexture);

    // Set blend mode to blend for transparency
    SDL_SetTextureBlendMode(colorTexture, SDL_BLENDMODE_BLEND);

    Uint8 r = (color >> 24) & 0xFF; // Shift right 24 bits to get the red component
    Uint8 g = (color >> 16) & 0xFF; // Shift right 16 bits to get the green component
    Uint8 b = (color >> 8) & 0xFF;  // Shift right 8 bits to get the blue component
    Uint8 a = color & 0xFF;      
    SDL_SetRenderDrawColor(renderer, r,g,b,a);

    // Clear the texture with the specified color
    SDL_RenderClear(renderer);

    // Reset the rendering target to the default
    SDL_SetRenderTarget(renderer, NULL);
    return colorTexture;
}