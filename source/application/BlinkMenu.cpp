#include "BlinkMenu.hpp"
#include "BLApplication.hpp"

#include <vector>

void BlinkMenu::run()
{
    // Set the background alpha of the next window (this is optional)
    ImGui::SetNextWindowBgAlpha(0.5f); // Adjust the alpha as needed

    // Set the initial position and size of the settings window (adjust as necessary)
    ImGui::SetNextWindowPos({(float)absoluteLayout.x, (float)absoluteLayout.y});
    ImGui::SetNextWindowSize({(float)absoluteLayout.w, (float)absoluteLayout.h});

    // Begin the settings window
    ImGui::Begin("Settings", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

    // Title for the settings category
    ImGui::Text("Game Settings");

    // Fullscreen toggle
    if (ImGui::Checkbox("Fullscreen", &BLApplication::isFullscreen)) {
        SDL_Event event;
        SDL_zero(event); // Initialize the event to zero
        event.type = BL_FULLSCREEN_TOGGLE;
        SDL_PushEvent(&event);
    }

    static int currentResolution = 0; // Variable to store the current selection, change it as per your needs or application state
    if (!BLApplication::isFullscreen)
    {
        if (ImGui::BeginCombo("Resolution", validResolutions[BLApplication::currentResolution].c_str())) {
            for (int i = 0; i < validResolutions.size(); i++) {
                bool isSelected = (BLApplication::currentResolution == i);
                if (ImGui::Selectable(validResolutions[i].c_str(), isSelected)) {
                    BLApplication::currentResolution = i; // Update the current resolution on selection
                    //BLApplication::instance()->changeWindow();
                    //BLApplication::changeWindow(BLApplication::instance()->resolutions[BLApplication::currentResolution]);
                    SDL_Event event;
                    SDL_zero(event); // Initialize the event to zero
                    event.type = BL_RESOLUTIONCHANGE;
                    SDL_PushEvent(&event);

                }

                // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                if (isSelected) {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }        
    }

    // Fullscreen toggle
    if (ImGui::Checkbox("Framerate", &BLApplication::frameRate)) {
    }
    /*
    // V-Sync toggle
    static bool isVsyncEnabled = false;
    ImGui::Checkbox("Enable V-Sync", &isVsyncEnabled);

    // Sound volume slider
    static float soundVolume = 0.5f; // Assuming 0.5 is the initial volume level
    ImGui::SliderFloat("Sound Volume", &soundVolume, 0.0f, 1.0f);*/

    // Add other settings as needed

    // End the settings window
    ImGui::End();
}

void BlinkMenu::init() {}

void BlinkMenu::applyResolution(int x, int y)
{
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
