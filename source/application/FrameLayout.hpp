#pragma once

#include <SDL.h>

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
};

