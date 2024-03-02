#pragma once

#include "imgui.h"

class BlinkMenu
{
public:
    BlinkMenu() = default;
    virtual ~BlinkMenu() = default;
    void init();
    void run();

    float scaleFactor = 1;
    ImVec2 context{1440,900};
};
