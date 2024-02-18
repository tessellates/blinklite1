#pragma once

#include "imgui.h"
#include <SDL.h>
#include <iostream>

using Vec2 = ImVec2;
using Rect = ImVec4;

inline void print(const SDL_Rect& rect)
{
    std::cout << "rect.x:" << rect.x << std::endl;
    std::cout << "rect.y:" << rect.y << std::endl;
    std::cout << "rect.w:" << rect.w << std::endl;
    std::cout << "rect.h:" << rect.h << std::endl;
}