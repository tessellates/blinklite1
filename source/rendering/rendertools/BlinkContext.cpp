#include <iostream>

#include "BlinkContext.hpp"
#include "SDLUtil.hpp"

void BlinkContext::applyResolution(int xResolution, int yResolution)
{
    if (pixelPerfectMode)
    {
        absoluteLayout.h = std::round(yResolution*frameLayout.ySize);
        absoluteLayout.h = (absoluteLayout.h/contextPixelSize.y)*contextPixelSize.y;
        if (absoluteLayout.h == 0)
        {
            absoluteLayout.h = contextPixelSize.y;
        }
    }
    else
    {
        absoluteLayout.h = std::round(yResolution*frameLayout.ySize);
    }
    absoluteLayout.w = std::round(absoluteLayout.h*frameLayout.xyRatio);
    absoluteLayout.y = -absoluteLayout.h/2+yResolution*frameLayout.y;
    absoluteLayout.x = -absoluteLayout.w/2+xResolution*frameLayout.x + 1;
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

Vec2 BlinkContext::pointInUnits(const SDL_Point& point) const
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

SDL_Rect BlinkContext::internalToAbsolute(const Rect& rect) const
{
    auto destf = rect;
    multiply_rect_x(destf, xScale);
    multiply_rect_y(destf, yScale);
    auto dest = cast_rect_smart(destf);
    dest.x += absoluteLayout.x;
    dest.y += absoluteLayout.y;
    return dest;
}

SDL_Point BlinkContext::internalToAbsolute(const Vec2& point) const
{
    SDL_Point result = {(int)(point.x*xScale), (int)(point.y*yScale)};
    return result;
}

void BlinkContext::print()
{
    std::cout << "height:" << absoluteLayout.h << std::endl;
    std::cout << "width:" << absoluteLayout.w << std::endl;
    std::cout << "x:" << absoluteLayout.x << std::endl;
    std::cout << "y:"<<  absoluteLayout.y << std::endl;
    std::cout << "pixely:" << absoluteLayout.h%contextPixelSize.y << std::endl;
    std::cout << "pixelx:" << absoluteLayout.w%contextPixelSize.x << std::endl;
    std::cout << "iux:" << internalUnits.x << std::endl;
    std::cout << "iuy:" << internalUnits.y << std::endl;
}