#include <iostream>

#include "BlinkContext.hpp"

void BlinkContext::applyResolution(int xResolution, int yResolution)
{
    absoluteLayout.h = yResolution*frameLayout.ySize;
    absoluteLayout.w = absoluteLayout.h*frameLayout.xyRatio;
    absoluteLayout.y = -absoluteLayout.h/2+yResolution*frameLayout.y;
    absoluteLayout.x = -absoluteLayout.w/2+xResolution*frameLayout.x;
    updateContext();
}

void BlinkContext::setInternalUnits(const SDL_Point& units)
{
    internalUnits = units;
    updateContext();
}

void BlinkContext::setFrameLayout(const FrameLayout& layout, int xResolution, int yResolution)
{
    frameLayout = layout;
    applyResolution(xResolution, yResolution);
}

void BlinkContext::updateContext()
{
    xScale = (float)absoluteLayout.w/(float)internalUnits.x;
    yScale = (float)absoluteLayout.h/(float)internalUnits.y;
}

Vec2 BlinkContext::pointInUnits(const SDL_Point& point)
{
    if (SDL_PointInRect(&point, &absoluteLayout)) 
    {
        float x = float(point.x - absoluteLayout.x)/xScale;
        float y = float(point.y - absoluteLayout.y)/yScale;
        return {x,y};
    }
    else
    {
        return {-1, -1};
    }
}

SDL_Rect