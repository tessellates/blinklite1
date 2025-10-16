#include "Engine_Fixed.hpp"
#include "TextureManager.hpp"
#include "QuadRenderer.hpp"
#include "Event.hpp"
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"
#include <iostream>
#include <stdexcept>

// ============================================================================
// Engine Implementation Only (TextureManager/QuadRenderer are in separate files)
// ============================================================================

// ============================================================================
// TextureManager Implementation
// ============================================================================

TextureManager::TextureManager(SDL_Renderer* renderer) : renderer(renderer) {
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

SDL_Texture* TextureManager::getTexture(TextureHandle handle) const {
    // Handle 0 is invalid, handles 1+ map to vector indices 0+
    if (handle == 0 || handle > textures.size()) {
        return nullptr;
    }
    return textures[handle - 1];
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
    SDL_Texture* texture = getTexture(handle);
    if (!texture) return {0, 0};
    
    int w, h;
    if (SDL_GetTextureSize(texture, &w, &h) < 0) {
        return {0, 0};
    }
    
    return {w, h};
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

// ============================================================================
// QuadRenderer Implementation  
// ============================================================================

QuadRenderer::QuadRenderer(SDL_Renderer* renderer, TextureManager& texManager)
    : renderer(renderer), textureManager(texManager) {
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
    // For now, simplified transform extraction
    // TODO: Proper matrix decomposition for complex transforms
    Transform transform = extractTransform(quad.mvp, {}); // Empty camera for now
    
    if (quad.tex != 0 && textureManager.isValid(quad.tex)) {
        renderTexturedQuad(quad, transform);
    } else {
        renderColorQuad(quad, transform);
    }
}

QuadRenderer::Transform QuadRenderer::extractTransform(const glm::mat4& mvp, const OrthoCamera& camera) const {
    // PROPER transform extraction (not your MVP abuse!)
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
    SDL_Texture* texture = textureManager.getTexture(quad.tex);
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

} // namespace BL

// ============================================================================
// Engine Implementation
// ============================================================================

struct Engine::Impl {
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    bool running = false;
    
    ~Impl() {
        cleanup();
    }
    
    void cleanup() {
        if (renderer) {
            SDL_DestroyRenderer(renderer);
            renderer = nullptr;
        }
        if (window) {
            SDL_DestroyWindow(window);
            window = nullptr;
        }
    }
};

Engine::Engine() : pimpl(std::make_unique<Impl>()) {}

Engine::~Engine() {
    quit(); // Ensure cleanup
}

bool Engine::init(const EngineConfig& config) {
    SDL_SetAppMetadata(config.title, config.version, config.id);
    SDL_SetHint(SDL_HINT_RENDER_VSYNC, config.vsync ? "1" : "0");
    
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return false;
    }
    
    if (!SDL_CreateWindowAndRenderer(config.title, config.w, config.h, 0, 
                                   &pimpl->window, &pimpl->renderer)) {
        SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
        return false;
    }
    
    // Initialize resource managers
    try {
        textureManager = std::make_unique<BL::TextureManager>(pimpl->renderer);
        quadRenderer = std::make_unique<BL::QuadRenderer>(pimpl->renderer, *textureManager);
    } catch (const std::exception& e) {
        SDL_Log("Failed to initialize resource managers: %s", e.what());
        return false;
    }
    
    // Initialize ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplSDL3_InitForSDLRenderer(pimpl->window, pimpl->renderer);
    ImGui_ImplSDLRenderer3_Init(pimpl->renderer);
    
    // Set VSync
    SDL_SetRenderVSync(pimpl->renderer, config.vsync ? 1 : 0);
    
    // Initialize TTF
    if (!TTF_Init()) {
        SDL_Log("TTF_Init failed: %s", SDL_GetError());
        // Non-fatal, continue
    }
    
    pimpl->running = true;
    return true;
}

void Engine::quit() {
    if (pimpl && pimpl->running) {
        pimpl->running = false;
        
        // Cleanup resource managers first
        quadRenderer.reset();
        textureManager.reset();
        
        // Cleanup ImGui
        ImGui_ImplSDLRenderer3_Shutdown();
        ImGui_ImplSDL3_Shutdown();
        ImGui::DestroyContext();
        
        // Cleanup TTF
        TTF_Quit();
        
        // Cleanup SDL (handled by Impl destructor)
        pimpl->cleanup();
        SDL_Quit();
    }
}

void Engine::iterate(double dt, std::span<const EventData> events) {
    if (!pimpl || !pimpl->running) return;
    
    // Handle engine-specific events
    handleEngineEvents(events);
    
    // Call game callbacks
    if (callbacks.tick) {
        callbacks.tick(static_cast<float>(dt), events);
    }
    
    // Extract render data
    renderSnapshots.clear();
    if (callbacks.extract) {
        callbacks.extract(renderSnapshots);
    }
    
    // Render frame
    SDL_SetRenderDrawColor(pimpl->renderer, 0, 0, 0, 255); // Black background
    SDL_RenderClear(pimpl->renderer);
    
    for (const auto& snapshot : renderSnapshots) {
        // Execute custom render callback first (for ImGui, etc.)
        if (snapshot.render) {
            snapshot.render();
        }
        
        // Render quads
        quadRenderer->render(snapshot);
    }
    
    SDL_RenderPresent(pimpl->renderer);
}

void Engine::handleEngineEvents(std::span<const EventData> events) {
    for (const auto& event : events) {
        switch (event.action) {
            case Event::FullscreenToggle:
                toggleFullscreen();
                break;
            case Event::Quit:
                pimpl->running = false;
                break;
            default:
                // Let game handle other events
                break;
        }
    }
}

void Engine::toggleFullscreen() {
    if (!pimpl || !pimpl->window) return;
    
    Uint32 flags = SDL_GetWindowFlags(pimpl->window);
    bool currentlyFullscreen = flags & SDL_WINDOW_FULLSCREEN;
    
    SDL_SetWindowFullscreen(pimpl->window, currentlyFullscreen ? 0 : SDL_WINDOW_FULLSCREEN);
    isFullscreen = !currentlyFullscreen;
}

TextureHandle Engine::loadBMPTexture(const char* path) {
    return textureManager ? textureManager->loadBMP(path) : 0;
}

TextureHandle Engine::loadBMPTextureWithKey(const char* path, const glm::ivec3& colorKey) {
    return textureManager ? textureManager->loadBMPWithColorKey(path, colorKey) : 0;
}

void* Engine::getRenderer() {
    return pimpl ? static_cast<void*>(pimpl->renderer) : nullptr;
}