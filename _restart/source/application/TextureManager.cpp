#include "TextureManager.hpp"
#include <SDL3/SDL.h>
#include "../sdl/bmp_load.hpp"
#include <stdexcept>

TextureManager::TextureManager(void* sdlRenderer) 
    : renderer(static_cast<SDL_Renderer*>(sdlRenderer)) {
    if (!renderer) {
        throw std::invalid_argument("TextureManager: renderer cannot be null");
    }
}

TextureManager::~TextureManager() {
    destroyAll();
}

TextureHandle TextureManager::loadBMP(const char* path) {
    if (!path) return 0;
    
    SDL_Texture* texture = LoadBMPTexture(renderer, path);
    if (!texture) {
        SDL_Log("Failed to load BMP texture: %s", path);
        return 0;
    }
    
    return addTexture(texture);
}

TextureHandle TextureManager::loadBMPWithColorKey(const char* path, const glm::ivec3& colorKey) {
    if (!path) return 0;
    
    SDL_Color key = {
        static_cast<Uint8>(colorKey.r), 
        static_cast<Uint8>(colorKey.g), 
        static_cast<Uint8>(colorKey.b), 
        255
    };
    
    SDL_Texture* texture = LoadBMPTextureWithKey(renderer, path, key);
    if (!texture) {
        SDL_Log("Failed to load BMP texture with color key: %s", path);
        return 0;
    }
    
    return addTexture(texture);
}

TextureHandle TextureManager::createWhitePixel() {
    // Create a 1x1 white texture for primitive rendering
    SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, 
                                           SDL_TEXTUREACCESS_STATIC, 1, 1);
    if (!texture) {
        SDL_Log("Failed to create white pixel texture: %s", SDL_GetError());
        return 0;
    }
    
    Uint32 white = 0xFFFFFFFF; // White pixel
    SDL_UpdateTexture(texture, nullptr, &white, 4);
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    
    return addTexture(texture);
}

void* TextureManager::getTexture(TextureHandle handle) const {
    // Handle 0 is invalid, handles 1+ map to vector indices 0+
    if (handle == 0 || handle > textures.size()) {
        return nullptr;
    }
    return static_cast<void*>(textures[handle - 1]);
}

bool TextureManager::isValid(TextureHandle handle) const {
    return handle != 0 && handle <= textures.size() && textures[handle - 1] != nullptr;
}

void TextureManager::destroy(TextureHandle handle) {
    if (handle == 0 || handle > textures.size()) {
        return;
    }
    
    SDL_Texture* texture = textures[handle - 1];
    if (texture) {
        SDL_DestroyTexture(texture);
        textures[handle - 1] = nullptr; // Mark as destroyed but keep slot
    }
}

void TextureManager::destroyAll() {
    for (SDL_Texture* texture : textures) {
        if (texture) {
            SDL_DestroyTexture(texture);
        }
    }
    textures.clear();
}

glm::ivec2 TextureManager::getSize(TextureHandle handle) const {
    SDL_Texture* texture = static_cast<SDL_Texture*>(getTexture(handle));
    if (!texture) return {0, 0};
    
    float w, h;
    if (SDL_GetTextureSize(texture, &w, &h) < 0) {
        return {0, 0};
    }
    
    return {(int)w, (int)h};
}

TextureHandle TextureManager::addTexture(SDL_Texture* texture) {
    if (!texture) return 0;
    
    // Look for empty slot first (from destroyed textures)
    for (size_t i = 0; i < textures.size(); ++i) {
        if (textures[i] == nullptr) {
            textures[i] = texture;
            return static_cast<TextureHandle>(i + 1); // Handle = index + 1
        }
    }
    
    // No empty slot, add to end
    textures.push_back(texture);
    return static_cast<TextureHandle>(textures.size()); // Handle = size (1-based)
}