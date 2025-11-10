#pragma once

#include <SDL3/SDL.h>
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include <cmath>
#include <algorithm>

enum Anchor
{
    CENTER,
    NEGATIVE,
    POSITIVE
};

struct FrameLayout
{
    float x = 0.5; 
    float y = 0.5;
    float ySize = 1;
    float xyRatio = 1;
    Anchor xAnchor = CENTER;
    Anchor yAnchor = CENTER;
    bool pixelPerfectMode = false;
    glm::ivec2 contextPixelSize = {1,1};
};

inline glm::ivec4 createAbsoluteLayout(const FrameLayout& f, int xRes, int yRes) {
    int h = std::lround(yRes * f.ySize);
    if (f.pixelPerfectMode) {
        const int step = std::max(1, f.contextPixelSize.y);
        h = std::max(step, (h / step) * step);
    }

    int w = std::lround(h * f.xyRatio);
    w = std::min(w, xRes);

    int x = 0, y = 0;

    switch (f.xAnchor) {
        case Anchor::CENTER:   x = std::lround(xRes * f.x - w * 0.5f); break;
        case Anchor::NEGATIVE: x = std::lround(xRes * f.x);            break;
        case Anchor::POSITIVE: x = std::lround(xRes - xRes * f.x - w); break;
    }
    switch (f.yAnchor) {
        case Anchor::CENTER:   y = std::lround(yRes * f.y - h * 0.5f); break;
        case Anchor::NEGATIVE: y = std::lround(yRes * f.y);            break;
        case Anchor::POSITIVE: y = std::lround(yRes - yRes * f.y - h); break;
    }

    return {x, y, w, h};
}

// convenience overload: resolution as ivec2
inline glm::ivec4 createAbsoluteLayout(const FrameLayout& f, glm::ivec2 res) {
    return createAbsoluteLayout(f, res.x, res.y);
}