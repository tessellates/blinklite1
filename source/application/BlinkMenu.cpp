#include "BlinkMenu.hpp"

void BlinkMenu::run()
{
    // Set the background alpha of the next window (this is optional)
    ImGui::SetNextWindowBgAlpha(0.5f); // Adjust the alpha as needed

    // Set the initial position and size of the settings window (adjust as necessary)
    ImGui::SetNextWindowPos(ImVec2(50, 50), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiCond_FirstUseEver);

    // Begin the settings window
    ImGui::Begin("Settings", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

    // Title for the settings category
    ImGui::Text("Game Settings");

    // Dummy resolution options
    const char* resolutions[] = { "800x600", "1024x768", "1280x720", "1920x1080" };
    static int currentResolution = 0; // Variable to store the current selection, change it as per your needs or application state
    ImGui::Combo("Resolution", &currentResolution, resolutions, IM_ARRAYSIZE(resolutions));

    // Fullscreen toggle
    static bool isFullscreen = false;
    ImGui::Checkbox("Fullscreen", &isFullscreen);

    // V-Sync toggle
    static bool isVsyncEnabled = false;
    ImGui::Checkbox("Enable V-Sync", &isVsyncEnabled);

    // Sound volume slider
    static float soundVolume = 0.5f; // Assuming 0.5 is the initial volume level
    ImGui::SliderFloat("Sound Volume", &soundVolume, 0.0f, 1.0f);

    // Add other settings as needed

    // End the settings window
    ImGui::End();
}

void BlinkMenu::init() {}
