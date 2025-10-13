#include "BlinkMenu.hpp"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"
#include <vector>

#include "Engine.hpp"
#include "EventStack.hpp"
#include "CoreTypes.hpp"

void BlinkMenu::run(SDL_Renderer* renderer)
{
    ImGui_ImplSDLRenderer3_NewFrame();
    ImGui_ImplSDL3_NewFrame();  
    //applyResolution(display.w, display.h);
    ImGui::NewFrame();
    internals();
    ImGui::Render();
    //SDL_SetRenderLogicalPresentation(renderer, 800, 600, SDL_LOGICAL_PRESENTATION_LETTERBOX);
    ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);
}

void BlinkMenu::internals()
{


    
    // Set the background alpha of the next window (this is optional)
    ImGui::SetNextWindowBgAlpha(0.5f); // Adjust the alpha as needed

    // Set the initial position and size of the settings window (adjust as necessary)
    ImGui::SetNextWindowPos({(float)absoluteLayout.x, (float)absoluteLayout.y});
    ImGui::SetNextWindowSize({(float)absoluteLayout.z, (float)absoluteLayout.w});

    // Begin the settings window
    ImGui::Begin("Settings", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

    // Title for the settings category
    ImGui::Text("Game Settings");

    if (isInitialized)
    {
        if (ImGui::Checkbox("Fullscreen", &engine->isFullscreen)) {
            eventStack->next.push_back({Event::FullscreenToggle, nullptr});
        }
    }

    // Fullscreen toggle
    if (ImGui::Checkbox("Framerate", &this->frameRate)) 
    {
        eventStack->next.push_back({Event::FrameRateToggle, nullptr});
    }
    /*
    // V-Sync toggle
    static bool isVsyncEnabled = false;
    ImGui::Checkbox("Enable V-Sync", &isVsyncEnabled);
    */
    // Sound volume slider
    //static float soundVolume = 0.5f; // Assuming 0.5 is the initial volume level
    //ImGui::SliderFloat("Sound Volume", &soundVolume, 0.0f, 1.0f);

    // Add other settings as needed

    // End the settings window
    ImGui::End();
}

void BlinkMenu::applyResolution(int x, int y)
{
    layout.xyRatio = float(x) / float(y);
    absoluteLayout = createAbsoluteLayout(layout, x, y);
}

void BlinkMenu::addResolutions(const std::vector<std::pair<int,int>>& sizes)
{
    validResolutions = std::vector<std::string>();
    for (const auto& res : sizes) 
    {
        validResolutions.push_back(std::to_string(res.first) + "x" + std::to_string(res.second));
    }
}

void BlinkMenu::extract( RenderSnapshots2D& s)
{
    RenderSnapshot2D rs;
    rs.render = [this]()
    {
        this->run((SDL_Renderer*)this->engine->getRenderer());
    };
    s.push_back(rs);
}