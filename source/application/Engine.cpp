#include "Engine.hpp"
#include "Event.hpp"
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"
#include <iostream>

struct Engine::Impl {
    SDL_Window*   win{};
    SDL_Renderer* ren{};
    bool running = false;
};

bool Engine::init(const EngineConfig& config) {
    SDL_SetAppMetadata(config.title, config.version, config.id);
    SDL_SetHint(SDL_HINT_RENDER_VSYNC, "0");
    p_ = new Impl();

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return false;
    }

    if (!SDL_CreateWindowAndRenderer(config.title, config.w, config.h, 0, &p_->win, &p_->ren)) {
        SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
        return false;
    }
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplSDL3_InitForSDLRenderer(p_->win, p_->ren);
    ImGui_ImplSDLRenderer3_Init(p_->ren);
    SDL_SetRenderVSync(p_->ren, 0);  // uncapped

    int vs = 12345;
    if (!SDL_GetRenderVSync(p_->ren, &vs))
        std::cout<<"GetRenderVSync failed: " <<SDL_GetError() << std::endl;
    else
        std::cout<<"VSync =" << vs << " (0=off, 1+=on, -1=adaptive)" <<std::endl;

    TTF_Init();
    // Store necessary state in Impl
    return true; // return false on failure
}

void Engine::quit() {
    if (p_) {
        ImGui_ImplSDLRenderer3_Shutdown();
        ImGui_ImplSDL3_Shutdown();
        ImGui::DestroyContext();

        if (p_->ren) SDL_DestroyRenderer(p_->ren);
        if (p_->win) SDL_DestroyWindow(p_->win);
        SDL_Quit();
        delete p_;
        p_ = nullptr;
    }
}

void Engine::iterate(double dt, std::span<const EventData> actions) {
    // Process input actions and update game state here
    // For now, just clear the screen with a color

    for (const auto& a : actions) {
        if (a.action == Event::FullscreenToggle) {
            toggleFullscreen();
        }
    }


    if (callbacks.step) {
        callbacks.step(dt, actions, renderSnapshot); // Placeholder call to step callback
    }
    
    if (p_ && p_->ren) {
        SDL_SetRenderDrawColor(p_->ren, 255, 255, 255, 255);
        SDL_RenderClear(p_->ren);
        if (callbacks.render)
        {
            callbacks.render(renderSnapshot);
        }
        SDL_RenderPresent(p_->ren);
    }
}

void* Engine::getRenderer() {
    return p_ ? static_cast<void*>(p_->ren) : nullptr;
}

void Engine::toggleFullscreen() 
{
    Uint32 isFullscreen = SDL_GetWindowFlags(p_->win) & SDL_WINDOW_FULLSCREEN;
    SDL_SetWindowFullscreen(p_->win, isFullscreen ? 0 : SDL_WINDOW_FULLSCREEN);
    this->isFullscreen = !isFullscreen;
    /*
    if (!BLApplication::isFullscreen)
    {
        BLApplication::changeWindow(resolutions[BLApplication::currentResolution]);
    }
    else
    {
        correctDisplay();
    }*/
}