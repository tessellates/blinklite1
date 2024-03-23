#pragma once

#include "imgui.h"
#include "FrameLayout.hpp"

#include <vector>

class BlinkMenu
{
public:
    BlinkMenu() = default;
    virtual ~BlinkMenu() = default;
    void applyResolution(int xResolution, int yResolution);
    void addResolutions(const std::vector<std::pair<int,int>>&);
    void init();
    void run();

    float scaleFactor = 1;
    FrameLayout layout = {0.5, 0.5, 0.95, 0.4, CENTER, CENTER};
    SDL_Rect absoluteLayout;

    std::vector<std::string> validResolutions;
};
