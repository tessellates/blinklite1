#include "QuadRenderer.hpp"
#include <SDL3/SDL.h>
#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>
#include  <stdexcept>

QuadRenderer::QuadRenderer(void* sdlRenderer, TextureManager& texManager)
    : renderer(static_cast<SDL_Renderer*>(sdlRenderer)), textureManager(texManager) {
    if (!renderer) {
        throw std::invalid_argument("QuadRenderer: renderer cannot be null");
    }
}

void QuadRenderer::render(const RenderSnapshot2D& snapshot) {
    // Sort quads by sort key for proper layering and batching
    auto sortedQuads = snapshot.quads;
    std::sort(sortedQuads.begin(), sortedQuads.end(), 
              [](const QuadCmd& a, const QuadCmd& b) {
                  return a.sortKey < b.sortKey;
              });
    
    // Render each quad
    for (const auto& quad : sortedQuads) {
        renderQuad(quad);
    }
}

void QuadRenderer::renderQuad(const QuadCmd& quad) {
    // Extract transform from MVP matrix
    Transform transform = extractTransform(quad.mvp, {}); // Empty camera for now
    
    if (quad.tex != 0 && textureManager.isValid(quad.tex)) {
        renderTexturedQuad(quad, transform);
    } else {
        renderColorQuad(quad, transform);
    }
}

QuadRenderer::Transform QuadRenderer::extractTransform(const glm::mat4& mvp, const OrthoCamera& camera) const {
    // PROPER transform extraction (not matrix element abuse!)
    Transform transform;
    
    // Extract translation from the 4th column
    transform.position = glm::vec2(mvp[3][0], mvp[3][1]);
    
    // Extract scale from the diagonal (assuming no rotation for now)
    // For proper decomposition, you'd need to handle rotation too
    transform.scale = glm::vec2(mvp[0][0], mvp[1][1]);
    
    // Extract rotation (simplified - assumes 2D rotation only)
    transform.rotation = 0.0f; // TODO: Implement proper rotation extraction
    
    return transform;
}

void QuadRenderer::renderTexturedQuad(const QuadCmd& quad, const Transform& transform) {
    SDL_Texture* texture = static_cast<SDL_Texture*>(textureManager.getTexture(quad.tex));
    if (!texture) return;
    
    // Set texture color modulation
    SDL_SetTextureColorMod(texture, 
                          static_cast<Uint8>(quad.color.r * 255),
                          static_cast<Uint8>(quad.color.g * 255), 
                          static_cast<Uint8>(quad.color.b * 255));
    SDL_SetTextureAlphaMod(texture, static_cast<Uint8>(quad.color.a * 255));
    
    // Source rectangle (UV mapping)
    glm::ivec2 texSize = textureManager.getSize(quad.tex);
    SDL_FRect srcRect = {
        quad.uv.x * texSize.x,
        quad.uv.y * texSize.y,
        (quad.uv.z - quad.uv.x) * texSize.x,
        (quad.uv.w - quad.uv.y) * texSize.y
    };
    
    // Destination rectangle
    SDL_FRect destRect = {
        transform.position.x,
        transform.position.y,
        transform.scale.x,
        transform.scale.y
    };
    
    // Render with proper UV mapping
    if (transform.rotation != 0.0f) {
        // Use SDL_RenderTextureRotated for rotation
        SDL_FPoint center = {destRect.w * 0.5f, destRect.h * 0.5f};
        SDL_RenderTextureRotated(renderer, texture, &srcRect, &destRect, 
                               glm::degrees(transform.rotation), &center, SDL_FLIP_NONE);
    } else {
        SDL_RenderTexture(renderer, texture, &srcRect, &destRect);
    }
}

void QuadRenderer::renderColorQuad(const QuadCmd& quad, const Transform& transform) {
    // Render colored rectangle
    SDL_SetRenderDrawColor(renderer,
                          static_cast<Uint8>(quad.color.r * 255),
                          static_cast<Uint8>(quad.color.g * 255),
                          static_cast<Uint8>(quad.color.b * 255),
                          static_cast<Uint8>(quad.color.a * 255));
    
    SDL_FRect rect = {
        transform.position.x,
        transform.position.y, 
        transform.scale.x,
        transform.scale.y
    };
    
    SDL_RenderFillRect(renderer, &rect);
}