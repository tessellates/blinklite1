#include "BLSDLTextureManager.hpp"

BLSDLTextureManager::~BLSDLTextureManager()
{
    for (SDL_Texture* texture : textures) {
        SDL_DestroyTexture(texture);
    }
}

SDL_Texture* BLSDLTextureManager::getTexture(size_t textureID) const
{
    return textures[textureID];
}

size_t BLSDLTextureManager::addTexture(SDL_Texture* texture)
{
    textures.push_back(texture);
    return textures.size() - 1;
}
