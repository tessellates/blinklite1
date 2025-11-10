#include "SDL3Backend.hpp"
#include "TextureManager.hpp"
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include "EventStack.hpp"
#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"

SDL3Backend::SDL3Backend() = default;

SDL3Backend::~SDL3Backend() {
    //shutdownImpl();
}

bool SDL3Backend::initImpl(const EngineConfig& config) {
    SDL_SetAppMetadata(config.title, config.version, config.id);
    SDL_SetHint(SDL_HINT_RENDER_VSYNC, config.vsync ? "1" : "0");
    
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return false;
    }
    
    if (!SDL_CreateWindowAndRenderer(config.title, config.w, config.h, 0, 
                                   &window, &renderer)) {
        SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
        return false;
    }
        
    // Initialize texture manager
    try {
        textureManager = std::make_unique<TextureManager>(renderer);
    } catch (const std::exception& e) {
        SDL_Log("Failed to initialize texture manager: %s", e.what());
        return false;
    }
    
    // Initialize ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer3_Init(renderer);

    // Set VSync
    SDL_SetRenderVSync(renderer, config.vsync ? 1 : 0);

    // Initialize TTF
    if (!TTF_Init()) {
        SDL_Log("TTF_Init failed: %s", SDL_GetError());
    }
    
    running = true;
    return true;
}

void SDL3Backend::shutdownImpl() {
    if (running) {
        running = false;
        //textureManager.reset(); // Why cannot reset without crashing?

        // Cleanup ImGui
        ImGui_ImplSDLRenderer3_Shutdown();
        ImGui_ImplSDL3_Shutdown();
        ImGui::DestroyContext();
        TTF_Quit();

        if (renderer) {
            SDL_DestroyRenderer(renderer);
            renderer = nullptr;
        }
        if (window) {
            SDL_DestroyWindow(window);
            window = nullptr;
        }

        SDL_Quit();
    }
}

void SDL3Backend::beginFrameImpl() {
    if (!renderer) return;
    
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
}

void SDL3Backend::endFrameImpl() {
    if (!renderer) return;
    
    SDL_RenderPresent(renderer);
}

void SDL3Backend::renderImpl(const std::vector<RenderSnapshot2D>& snapshots) {
    if (!renderer) return;
    
    for (const auto& snapshot : snapshots) {
        std::cout << "here\n" << std::endl;
        // Apply viewport
        if (snapshot.context.viewport.z > 0 && snapshot.context.viewport.w > 0) {
            SDL_Rect viewport = {
                static_cast<int>(snapshot.context.viewport.x),
                static_cast<int>(snapshot.context.viewport.y), 
                static_cast<int>(snapshot.context.viewport.z),
                static_cast<int>(snapshot.context.viewport.w)
            };
            SDL_SetRenderViewport(renderer, &viewport);
        }
        
        // Render all quads directly (no transformation)
        for (const auto& quad : snapshot.quads) {
            renderQuad(quad);
        }
        
        if (snapshot.customRender) {
            snapshot.customRender();
        }
    }
    SDL_SetRenderViewport(renderer, nullptr);
}

void SDL3Backend::renderQuad(const QuadCmd& quad) {
    if (quad.tex == 0) {
        renderColoredQuad(quad);
    } else {
        renderTexturedQuad(quad);
    }
}

void SDL3Backend::renderColoredQuad(const QuadCmd& quad) {
    SDL_FRect rect = {
        quad.position.x,
        quad.position.y,
        quad.size.x,
        quad.size.y
    };
    
    SDL_SetRenderDrawColor(renderer,
        static_cast<Uint8>(quad.color.r * 255),
        static_cast<Uint8>(quad.color.g * 255),
        static_cast<Uint8>(quad.color.b * 255),
        static_cast<Uint8>(quad.color.a * 255));
    SDL_RenderFillRect(renderer, &rect);
}

void SDL3Backend::renderTexturedQuad(const QuadCmd& quad) {
    if (!textureManager) return;
    
    // Get the texture directly (no atlas)
    SDL_Texture* texture = textureManager->getTexture(quad.tex);
    if (!texture) return;
    /*
    // Apply color tint
    SDL_SetTextureColorMod(texture,
        static_cast<Uint8>(quad.color.r * 255),
        static_cast<Uint8>(quad.color.g * 255),
        static_cast<Uint8>(quad.color.b * 255));
    SDL_SetTextureAlphaMod(texture, 
        static_cast<Uint8>(quad.color.a * 255));*/
    
    // Get texture size for UV conversion
    float texW, texH;
    SDL_GetTextureSize(texture, &texW, &texH);
    
    // Convert normalized UV to pixel coordinates
    SDL_FRect srcRect = {
        quad.uv.x * texW,
        quad.uv.y * texH,
        (quad.uv.z - quad.uv.x) * texW,
        (quad.uv.w - quad.uv.y) * texH
    };
    
    // Extract destination rectangle from MVP matrix
    //glm::vec4 pos = quad.mvp[3]; // Translation
    //glm::vec3 scaleX = quad.mvp[0]; // X scale  
    //glm::vec3 scaleY = quad.mvp[1]; // Y scale
    
    SDL_FRect destRect = {
        quad.position.x,
        quad.position.y,
        quad.size.x,
        quad.size.y
    };
    
    // Simple rendering without rotation for now
    SDL_RenderTexture(renderer, texture, &srcRect, &destRect);
}

TextureHandle SDL3Backend::loadTextureImpl(const char* path) {
    return textureManager ? textureManager->loadPNGTexture(path) : 0;
}

glm::ivec2 SDL3Backend::getWindowSizeImpl() const {
    if (window) {
        int w, h;
        SDL_GetWindowSize(window, &w, &h);
        return glm::ivec2(w, h);
    }
    return glm::ivec2(0, 0);
}

void* SDL3Backend::getNativeRendererImpl() {
    return static_cast<void*>(renderer);
}

void SDL3Backend::toggleFullscreenImpl() {
    if (!window) return;
    
    Uint32 flags = SDL_GetWindowFlags(window);
    bool currentlyFullscreen = flags & SDL_WINDOW_FULLSCREEN;
    
    SDL_SetWindowFullscreen(window, currentlyFullscreen ? 0 : SDL_WINDOW_FULLSCREEN);
    
    // Notify about window resize
    EventStack::instance()->pushNow(Event::WindowResized, nullptr);
}