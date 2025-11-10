#include "TextureManager.hpp"
#include <SDL3/SDL.h>
#include <stdexcept>
#include <SDL3_image/SDL_image.h>
#include <iostream>

TextureManager::TextureManager(void* sdlRenderer)
    : renderer(static_cast<SDL_Renderer*>(sdlRenderer)) {
    if (!renderer) {
        throw std::invalid_argument("TextureManager: renderer cannot be null");
    }
    
    // Reserve slot 0 as invalid
    textures = std::vector<TextureInfo>();
    textures.push_back({nullptr, {0, 0}});
}

TextureManager::~TextureManager() {
    destroyAll();
}

SDL_Texture* TextureManager::getTexture(TextureHandle handle) const
{
    if (!isValid(handle)) return nullptr; // TO-DO create default texture to return
    return textures[handle].texture;
}

const TextureInfo& TextureManager::getTextureInfo(TextureHandle handle) const 
{
    if (!isValid(handle)) return textures[0];
    return textures[handle];
}

TextureHandle TextureManager::loadPNGTexture(const char* path) {
    if (!path) return 0;
    
    SDL_Texture* texture = IMG_LoadTexture(renderer, path);
    if (!texture) {
        SDL_Log("Failed to load PNG texture: %s", path);
        return 0;
    }
    
    float w, h;
    SDL_GetTextureSize(texture, &w, &h);
    
    TextureHandle handle = addTexture(texture, {(int)w, (int)h});
    
    return handle;
}

TextureHandle TextureManager::addTexture(SDL_Texture* texture, const glm::ivec2& size) 
{
    textures.push_back({texture, size});
    return static_cast<TextureHandle>(textures.size() - 1);
}

bool TextureManager::isValid(TextureHandle handle) const {
    if (handle == 0 || handle >= textures.size()) return false;
    const TextureInfo& info = textures[handle];
    return info.texture != nullptr;
}

void TextureManager::destroy(TextureHandle handle) {
    if (!isValid(handle)) return;
    
    TextureInfo& info = textures[handle];
    if (info.texture) {
        SDL_DestroyTexture(info.texture);
        info.texture = nullptr;
    }
}

void TextureManager::destroyAll()
{
    std::cout << "Destroying all textures" << std::endl;
    std::cout << textures.size() << " textures to destroy" << std::endl;
    /*
    for (auto& info : textures) {
        std::cout << "Checking texture for destruction" << std::endl;
        if (info.texture) {
            std::cout << "Destroying texture" << std::endl;
            SDL_DestroyTexture(info.texture);
            info.texture = nullptr;
            std::cout << "Texture destroyed" << std::endl;
            
        }
    }
    std::cout << "All textures destroyed" << std::endl;
    textures.clear();
    textures.push_back({nullptr, {0, 0}});*/
}