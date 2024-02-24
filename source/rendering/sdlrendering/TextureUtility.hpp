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

void applyBloomEffect(std::vector<Uint32>& pixels, int w, int h, int rr = 0, int xc = 0, int yc = 0) {

    std::random_device rd;
    std::uniform_int_distribution<> distrib{1, 10};
    std::mt19937 gen = std::mt19937(rd());
    float mod = 1;
    std::vector<Uint32> tempPixels = pixels; // Work on a copy to avoid modifying the original data while reading it
    bool filter = false;
    if (rr > 0) filter = true;
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            if (filter)
            {
                mod = std::sqrt(rr)*float((x-xc)*(x-xc) + (y-yc)*(y-yc))/(rr*rr);
                mod = std::max<float>(std::sqrt(std::sqrt(mod)), 0.2);
                if (rr*rr < (x-xc)*(x-xc) + (y-yc)*(y-yc) )
                {
                    continue;
                }
            }
            // Example condition: Apply bloom if the pixel is not completely black
            if (pixels[y * w + x] != 0) {
                uint32_t& pixel = pixels[y * w + x];
                uint8_t r = (pixel >> 24) & 0xFF;
                uint8_t g = (pixel >> 16) & 0xFF;
                uint8_t b = (pixel >> 8) & 0xFF;
                int random = distrib(gen);

                int cuttoff = 200 - random*5;
                if (!(r > cuttoff || g > cuttoff || b > cuttoff))
                    continue;

                // Define the radius of the bloom effect
                int radius = 8; // Small radius for demonstration

                // Apply bloom effect within the radius
                for (int dy = -radius; dy <= radius; ++dy) {
                    for (int dx = -radius; dx <= radius; ++dx) {
                        int ny = y + dy;
                        int nx = x + dx;
                        int descale = (std::abs(dy) + std::abs(dx))/2;
                        descale = std::max(descale, 1);
                        if (nx >= 0 && nx < w && ny >= 0 && ny < h) {
                            // Simple bloom effect: Lighten the pixel by increasing its value
                            uint32_t& pixel = tempPixels[ny * w + nx];
                            uint8_t r = (pixel >> 24) & 0xFF;
                            uint8_t g = (pixel >> 16) & 0xFF;
                            uint8_t b = (pixel >> 8) & 0xFF;
                            int mod2 = (random/descale) * std::sqrt((float(r)/float(255))) / mod;
                            // Increase brightness by 10% for demonstration
                            r = std::min(255, r + mod2);
                            g = std::min(255, g + mod2/2);
                            b = std::min(255, b + mod2/2);

                            pixel = (r << 24) | (g << 16) | (b << 8) | (pixel & 0xFF);
                        }
                    }
                }
            }
        }
    }

    // Copy modified pixels back
    pixels = std::move(tempPixels);
}
