#pragma once
#include "CoreTypes.hpp"
#include <memory>
#include <vector>
#include <unordered_map>
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

// Forward declarations
struct SDL_Renderer;
struct SDL_Texture;

struct TextureInfo {
    SDL_Texture* texture = nullptr;
    glm::ivec2 size{0, 0};
    //glm::vec4 atlasUV{0, 0, 1, 1}; // Atlas coordinates (u0, v0, u1, v1)
    //TextureHandle atlasHandle = 0;  // Points to atlas texture, or 0 if standalone
    //bool isAtlased = false;
};

class TextureManager {
public:
    explicit TextureManager(void* sdlRenderer);
    ~TextureManager();
    
    // Non-copyable, moveable
    TextureManager(const TextureManager&) = delete;
    TextureManager& operator=(const TextureManager&) = delete;
    
    // Texture loading - these return handles that may point to atlas entries
    TextureHandle loadPNGTexture(const char* path);
    SDL_Texture* getTexture(TextureHandle handle) const;
    TextureInfo& getTextureInfo(TextureHandle handle) const;
    bool isValid(TextureHandle handle) const;

    void destroy(TextureHandle handle);
    void destroyAll();

private:
    SDL_Renderer* renderer;
    std::vector<TextureInfo> textures;
    
    void addTexture(SDL_Texture* texture, const glm::ivec2& size);
};