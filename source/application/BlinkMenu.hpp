#pragma once

#include "imgui.h"
#include "FrameLayout.hpp"
#include <SDL3/SDL.h>
#include <vector>
#include <string>

class BlinkMenu
{
public:
    BlinkMenu() = default;
    virtual ~BlinkMenu() = default;
    void applyResolution(int xResolution, int yResolution);
    void addResolutions(const std::vector<std::pair<int,int>>&);
    void init();
    void run(SDL_Renderer* renderer);
    void internals();

    float scaleFactor = 1;
    FrameLayout layout = {0.5, 0.5, 0.95, 0.95, CENTER, CENTER};
    glm::ivec4 absoluteLayout;

    std::vector<std::string> validResolutions;
};
