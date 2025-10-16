#pragma once

#include "Coordinate.hpp"
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

struct Grid
{
    glm::ivec4 rect;
    int nx;
    int ny;
    float xsize;
    float ysize;

    float csize;

    Grid() = default;
    Grid(const Rect& rect, int nx, int ny) : rect(rect), nx(nx), ny(ny)
    {
        xsize = (float)(rect.z)/nx;
        ysize = (float)(rect.w)/ny;
    }

    Coordinate coordinate(const glm::ivec2& point) const
    {
        glm::ivec2 relative = {int(point.x - rect.x), int(point.y - rect.y)};
        int x = (relative.x) / xsize;
        int y = (relative.y) / ysize;
        return Coordinate(x, y);
    }

    glm::vec2 point(const Coordinate& pos) const
    {
        return {rect.x + pos.x * xsize, rect.y + pos.y * ysize};
    }
};

inline bool pointInRect(const glm::ivec2& point, const glm::ivec4& rect) 
{
    return point.x >= rect.x && point.x <= (rect.x + rect.z) &&
           point.y >= rect.y && point.y <= (rect.y + rect.w);
}