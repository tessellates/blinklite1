#pragma once

#include "FrameLayout.hpp"
#include "BLDefs.hpp"

class BlinkContext {
public:
    BlinkContext() = default;
    void applyResolution(int xResolution, int yResolution);
    void setInternalUnits(const SDL_Point&);
    void setFrameLayout(const FrameLayout&, int, int);
    void updateContext();
    Vec2 pointInUnits(const SDL_Point& point) const;
    SDL_Rect internalToAbsolute(const Rect& rect) const;
    SDL_Point internalToAbsolute(const Vec2& point) const;

    //d
    void print();
public:
    FrameLayout frameLayout;
    SDL_Rect absoluteLayout;
    SDL_Point internalUnits;

    float xScale = 1;
    float yScale = 1;

    bool pixelPerfectMode = false;
    SDL_Point contextPixelSize = {1,1};
};
