#pragma once

#include <SDL.h>
#include "BLutils.hpp"

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
    SDL_Point contextPixelSize = {1,1};
};

inline SDL_Rect createAbsoluteLayout(const FrameLayout& frameLayout, int xResolution, int yResolution)
{
    SDL_Rect absoluteLayout;
    absoluteLayout.h = std::round(yResolution*frameLayout.ySize);

    if (frameLayout.pixelPerfectMode)
    {
        absoluteLayout.h = (absoluteLayout.h/frameLayout.contextPixelSize.y)*frameLayout.contextPixelSize.y;
        if (absoluteLayout.h == 0)
        {
            absoluteLayout.h = frameLayout.contextPixelSize.y;
        }
    }

    absoluteLayout.w = std::round(absoluteLayout.h*frameLayout.xyRatio);
    if (absoluteLayout.w > xResolution)
    {
        absoluteLayout.w = xResolution;
    }

    switch(frameLayout.xAnchor)
    {
        case CENTER:
        {
            absoluteLayout.x = -absoluteLayout.w/2+xResolution*frameLayout.x;
            break;
        }
        case NEGATIVE:
        {
            absoluteLayout.x = xResolution*frameLayout.x;
            break;
        }
        case POSITIVE:
        {
            absoluteLayout.x = xResolution-xResolution*frameLayout.x-absoluteLayout.w;
            break;
        }
    }

    switch(frameLayout.yAnchor)
    {
        case CENTER:
        {
            absoluteLayout.y = -absoluteLayout.h/2+yResolution*frameLayout.y;
            break;
        }
        case NEGATIVE:
        {
            absoluteLayout.y = yResolution*frameLayout.y;
            break;
        }
        case POSITIVE:
        {
            absoluteLayout.y = yResolution-yResolution*frameLayout.y-absoluteLayout.y;
            break;
        }
    }
    return absoluteLayout;
}
