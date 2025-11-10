#pragma once
#include "CoreTypes.hpp"
#include <memory>
#include <vector>
#include <glm/vec2.hpp>

// Forward declarations - no SDL dependencies in header!
struct SDL_Renderer;
struct SDL_Texture;

/**
 * Resource manager for textures with proper RAII and error handling.
 * Handles texture loading, management, and cleanup.
 * Uses simple vector-based handles for O(1) access.
 */
class TextureManager {
public:
    explicit TextureManager(void* sdlRenderer);
    ~TextureManager();
    
    // Non-copyable, moveable
    TextureManager(const TextureManager&) = delete;
    TextureManager& operator=(const TextureManager&) = delete;
    TextureManager(TextureManager&&) = default;
    TextureManager& operator=(TextureManager&&) = default;
    
    // Texture loading
    TextureHandle loadBMP(const char* path);
    TextureHandle loadBMPWithColorKey(const char* path, const glm::ivec3& colorKey);
    TextureHandle createWhitePixel(); // For primitive shape rendering
    
    // Texture access (returns opaque void* to avoid SDL dependency)
    void* getTexture(TextureHandle handle) const;
    bool isValid(TextureHandle handle) const;
    
    // Resource management
    void destroy(TextureHandle handle);
    void destroyAll();
    
    // Texture info
    glm::ivec2 getSize(TextureHandle handle) const;
    
private:
    SDL_Renderer* renderer;
    std::vector<SDL_Texture*> textures;
    
    TextureHandle addTexture(SDL_Texture* texture);
};