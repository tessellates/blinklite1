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
