#pragma once

#include "Coordinate.hpp"
#include "BLDefs.hpp"

struct Grid
{
    SDL_Rect rect;
    int nx;
    int ny;
    float xsize;
    float ysize;

    float csize;

    Grid() = default;
    Grid(const SDL_Rect& rect, int nx, int ny) : rect(rect), nx(nx), ny(ny)
    {
        xsize = (float)(rect.w)/nx;
        ysize = (float)(rect.h)/ny;
    }
/*
    Grid(const SDL_Rect& rect, float xsize, float ysize ) : rect(rect), xsize(xsize), ysize(ysize)
    {
        nx = rect.w/xsize;
        ny = rect.h/ysize;
    }*/

    Coordinate coordinate(const SDL_Point& point) const
    {
        SDL_Point relative = {point.x - rect.x, point.y - rect.y};
        int x = (relative.x) / xsize;
        int y = (relative.y) / ysize;
        return Coordinate(x, y);
    }

    Vec2 point(const Coordinate& pos) const
    {
        return {rect.x + pos.x * xsize, rect.y + pos.y * ysize};
    }
};
