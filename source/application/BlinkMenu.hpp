#pragma once

#include "imgui.h"

class BlinkMenu
{
public:
    BlinkMenu() = default;
    virtual ~BlinkMenu() = default;
    void init();
    void run();
};
