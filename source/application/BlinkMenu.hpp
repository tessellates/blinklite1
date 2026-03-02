#pragma once

#include "imgui.h"
#include "FrameLayout.hpp"
#include <SDL3/SDL.h>
#include <vector>
#include <string>
#include "BaseModule.hpp"

class EventStack;

class BlinkMenu : public BaseModule
{
public:
    BlinkMenu() = default;
    virtual ~BlinkMenu() = default;
    void applyResolution(int xResolution, int yResolution);
    void addResolutions(const std::vector<std::pair<int,int>>&);

    void extract( RenderSnapshots2D& s) override;

    float scaleFactor = 1;
    FrameLayout layout = {0.5, 0.5, 0.95, 0.95, CENTER, CENTER};
    glm::ivec4 absoluteLayout;
    std::vector<std::string> validResolutions;

public:
    void run(void* renderContext);
    void internals();
    bool isInit = false;
    bool frameRate = false;
};
