#include "Engine.hpp"
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
        textureManager = std::make_unique<TextureManager>(pimpl->renderer);
        quadRenderer = std::make_unique<QuadRenderer>(pimpl->renderer, *textureManager);
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
        
        // Render quads using our separated QuadRenderer
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