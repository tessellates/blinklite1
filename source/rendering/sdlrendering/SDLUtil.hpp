#pragma once

#include <SDL.h>

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