#pragma once

 #include <SDL3/SDL.h>
#include "BLutils.hpp"
#include <cmath>

inline void multiply_rect_x(SDL_FRect& rect, float multiplier)
{
    rect.x *= multiplier;
    rect.w *= multiplier;
}

inline void multiply_rect_y(SDL_FRect& rect, float multiplier)
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

inline SDL_FRect cast_rect(const Rect& rect)
{
    return SDL_FRect{(rect.x), (rect.y), (rect.z), (rect.w)};
}

inline SDL_FRect cast_rect_smart(const Rect& rect)
{
    SDL_FRect result = {(rect.x), (rect.y), (rect.z), (rect.w)};
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

inline SDL_Rect RoundRect(const SDL_FRect& frect) {
    return SDL_Rect{
        static_cast<int>(std::round(frect.x)),
        static_cast<int>(std::round(frect.y)),
        static_cast<int>(std::round(frect.w)),
        static_cast<int>(std::round(frect.h))
    };
}