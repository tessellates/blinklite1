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
    FrameLayout layout = {0.02, 0.02, 0.9, 0.5, NEGATIVE, NEGATIVE};
    ImVec2 absolutePos;
    ImVec2 absoluteSize;

    std::vector<std::string> validResolutions;
};
