#include <SDL.h>
#include <SDL_image.h>
#include <vector>
#include <cmath>
#include <random>
#include <iostream>


inline SDL_Texture* CreateTextureFromFile(SDL_Renderer* renderer, const char* filePath) {
    // Load the image into an SDL_Surface
    SDL_Surface* surface = IMG_Load(filePath);
    if (surface == nullptr) {
        SDL_Log("Unable to load image %s! SDL_image Error: %s\n", filePath, IMG_GetError());
        return nullptr;
    }

    // Convert the surface to a texture
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);  // RGBA: 0,0,0,0 for full transparency

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (texture == nullptr) {
        SDL_Log("Unable to create texture from %s! SDL Error: %s\n", filePath, SDL_GetError());
    }

    // Set blend mode to blend for transparency
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

    // Free the surface as it's no longer needed
    SDL_FreeSurface(surface);

    return texture;
}

inline SDL_Texture* CreateColorTexture(SDL_Renderer* renderer, Uint32 color) {
    // Create a texture that will be used as a render target
    SDL_Texture* colorTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 2, 2);

    // Set the texture as the current rendering target
    SDL_SetRenderTarget(renderer, colorTexture);

    // Set blend mode to blend for transparency
    SDL_SetTextureBlendMode(colorTexture, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);

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


inline SDL_Texture* CreateGridTexture(SDL_Renderer* renderer, SDL_Texture* tile, int nx, int ny) 
{

    int w,h;
    SDL_QueryTexture(tile, nullptr, nullptr, &w, &h);
    // Create a texture that will be used as a render target
    SDL_Texture* gridTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, w*nx, ny*h);

    // Set the texture as the current rendering target
    SDL_SetRenderTarget(renderer, gridTexture);
    SDL_SetRenderDrawColor(renderer, 0,0,0,0);
    SDL_RenderClear(renderer);
    // Set blend mode to blend for transparency
    SDL_SetTextureBlendMode(gridTexture, SDL_BLENDMODE_BLEND);


    // Render the grid of textures
    for (int i = 0; i < nx; ++i) {
        for (int j = 0; j < ny; ++j) {
            // Calculate the source rectangle for this cell
            //SDL_Rect srcRect = { i * cellWidth, j * cellHeight, cellWidth, cellHeight };

            // Calculate the destination rectangle for this cell
            SDL_Rect dstRect = { i * w, j * h, w, h };

            // Render the texture onto the screen
            SDL_RenderCopy(renderer, tile, nullptr, &dstRect);
        }
    }

    // Clear the texture with the specified color
    //SDL_RenderClear(renderer);

    // Reset the rendering target to the default
    SDL_SetRenderTarget(renderer, NULL);
    return gridTexture;
}