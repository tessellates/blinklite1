#pragma once
#include "CoreTypes.hpp"
#include "TextureManager.hpp"
#include <glm/vec2.hpp>

// Forward declarations - no SDL dependencies!
struct SDL_Renderer;

/**
 * Renders QuadCmd objects using SDL3.
 * Handles sorting, batching, and proper transform extraction.
 */
class RectRenderer {
public:
    explicit RectRenderer(void* sdlRenderer, TextureManager& texManager);
    
    // Render a complete snapshot
    void render(const RenderSnapshot2D& snapshot);
    
    // Render individual quad
    void renderRect(const RectCmd& quad);
    
private:
    SDL_Renderer* renderer;
    TextureManager& textureManager;
    
    // Transform extraction from MVP matrix
    struct Transform {
        glm::vec2 position;
        glm::vec2 scale;
        float rotation;
    };
    
    Transform extractTransform(const glm::mat4& mvp, const OrthoCamera& camera) const;
    void renderTexturedQuad(const QuadCmd& quad, const Transform& transform);
    void renderColorQuad(const QuadCmd& quad, const Transform& transform);
};