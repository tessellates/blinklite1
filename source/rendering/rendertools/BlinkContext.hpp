#pragma once

#include "FrameLayout.hpp"

class BlinkContext {
public:
    BlinkContext() = default;
    void render();
    void applyResolution(int xResolution, int yResolution);
    void setInternalUnits(const SDL_Point&);
    void setFrameLayout(const FrameLayout&, int, int);
    Vec2 pointInUnits(const SDL_Point& point);
    SDL_Rect 

public:
    FrameLayout frameLayout;
    SDL_Rect absoluteLayout;
    SDL_Point internalUnits;

    float xScale = 1;
    float yScale = 1;
};
