#include "TextureManager.hpp"
#include <SDL3/SDL.h>
#include <stdexcept>
#include "sdl_image_utils.hpp"

TextureManager::TextureManager(void* sdlRenderer)
    : renderer(static_cast<SDL_Renderer*>(sdlRenderer)) {
    if (!renderer) {
        throw std::invalid_argument("TextureManager: renderer cannot be null");
    }
    
    // Reserve slot 0 as invalid
    textures.push_back({});
}

        TextureManager::~TextureManager() {
    destroyAll();
}

SDL_Texture* TextureManager::getTexture(TextureHandle handle) const
{
    if (!isValid(handle)) return nullptr; // TO-DO create default texture to return
    return textures[handle].texture;
}

TextureInfo& TextureManager::getTextureInfo(TextureHandle handle) const 
{
    if (!isValid(handle)) return textures[0];
    return textures[handle];
}

TextureHandle TextureManager::loadPNGTexture(const char* path) {
    if (!path) return 0;
    
    SDL_Texture* texture = LoadPNGTexture(renderer, path);
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
    for (auto& info : textures) {
        if (info.texture) {
            SDL_DestroyTexture(info.texture);
            info.texture = nullptr;
        }
    }
    textures.clear();
    textures.push_back({});
}