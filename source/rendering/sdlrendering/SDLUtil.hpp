#pragma once

#include <SDL.h>
#include "BLDefs.hpp"
#include <cmath>

inline void multiply_rect_x(SDL_Rect& rect, float multiplier)
{
    rect.x *= multiplier;
    rect.w *= multiplier;
}

inline void multiply_rect_y(SDL_Rect& rect, float multiplier)
{
    rect.y *= multiplier;
    rect.h *= multiplier;
}

inline void multiply_rect_x(Rect& rect, float multiplier)
{
    rect.x *= multiplier;
    rect.z *= multiplier;
}

inline void multiply_rect_y(Rect& rect, float multiplier)
{
    rect.y *= multiplier;
    rect.w *= multiplier;
}

inline SDL_Rect cast_rect(const Rect& rect)
{
    return SDL_Rect{(int)(rect.x), (int)(rect.y), (int)(rect.z), (int)(rect.w)};
}

inline SDL_Rect cast_rect_smart(const Rect& rect)
{
    SDL_Rect result = {(int)(rect.x), (int)(rect.y), (int)(rect.z), (int)(rect.w)};
    if (int(rect.x+rect.z) > result.x + result.w)
    {
        result.w++;
        std::cout << "pixel-miss-detected" << std::endl;
    }
    if (int(rect.y+rect.w) > result.y + result.h)
    {
        result.h++;
        std::cout << "pixel-miss-detected" << std::endl;
    }
    return result;
}