#include <iostream>

#include <SDL.h>
#include <SDL_image.h>

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"

#include "RenderInfo.hpp"

int main() 
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
    {
        printf("Error: %s\n", SDL_GetError());
    }
/*
    //int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags)) {
        // Handle error
    }*/

        // From 2.0.18: Enable native IME.
#ifdef SDL_HINT_IME_SHOW_UI
    SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");
#endif

    // Create window with SDL_Renderer graphics context
    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_FULLSCREEN_DESKTOP);
    auto window_ = SDL_CreateWindow("BLINK 2D", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 0, 0, window_flags);
    auto renderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
    if (renderer_ == nullptr)
    {
        SDL_Log("Error creating SDL_Renderer!");
    }
/*
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    io_ = &ImGui::GetIO();
    //io_->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io_->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForSDLRenderer(window_, renderer_);
    ImGui_ImplSDLRenderer2_Init(renderer_);*/
     /*
     LOOP 
     */
    SDL_Event event_;
    //ImGuiIO* io_;
    bool done_ = false;
    while(!done_)
    {
        while (SDL_PollEvent(&event_))
        {
            //ImGui_ImplSDL2_ProcessEvent(&event_);
            if (event_.type == SDL_QUIT)
                done_ = true;
            if (event_.type == SDL_WINDOWEVENT && event_.window.event == SDL_WINDOWEVENT_CLOSE && event_.window.windowID == SDL_GetWindowID(window_))
                done_ = true;
        }
        SDL_RenderPresent(renderer_);
    }
    /*
     Cleanup 
     */
     /*
    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();*/

    SDL_DestroyRenderer(renderer_);
    SDL_DestroyWindow(window_);
    SDL_Quit();
    
    return 0;
}


