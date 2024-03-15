#pragma once

#include <SDL.h>
#include <vector>

class BLSDLTextureManager
{
public:
    BLSDLTextureManager() = default;
    ~BLSDLTextureManager();
    SDL_Texture* getTexture(size_t textureID) const;
    size_t addTexture(SDL_Texture* texture);
public:
    std::vector<SDL_Texture*> textures;
};