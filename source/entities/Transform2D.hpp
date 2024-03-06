#pragma once

#include "BLDefs.hpp"

#include <SDL.h>
#include <cmath> // For cosf() and sinf()

// Rotate this vector by a given angle (in degrees)
inline Vec2 rotate(const Vec2& vec, float angleDegrees)
{
    float angleRadians = angleDegrees * (M_PI / 180.0f);
    float cosAngle = cosf(angleRadians);
    float sinAngle = sinf(angleRadians);
    return Vec2
    {
        vec.x * cosAngle - vec.y * sinAngle,
        vec.x * sinAngle + vec.y * cosAngle
    };
}

struct Transform2D 
{
    Vec2 position = {0.0f, 0.0f}; 
    float rotation = {0.0f};
    SDL_RendererFlip flip = SDL_FLIP_NONE;
};

inline Transform2D calculateAbsoluteTransform(const Transform2D& child, const Transform2D& parent) {
    Transform2D absolute;

    // Rotation influenced by parent's rotation. Child's flip does not affect position or rotation calculation.
    absolute.rotation = child.rotation + parent.rotation;

    // Calculate the absolute position by rotating the child's position by the parent's rotation,
    // then adding the parent's position.
    absolute.position = rotate(child.position, parent.rotation) + parent.position;

    // Combine flips using XOR to toggle flip states appropriately.
    // This effectively toggles a flip state if it's different between parent and child.
    absolute.flip = static_cast<SDL_RendererFlip>(parent.flip ^ child.flip);

    // Adjust the rotation based on the parent's flip state.
    // Note: The effect of flip on rotation is conceptual; adjust this logic to fit your rendering model.
    if (parent.flip & SDL_FLIP_HORIZONTAL) {
        // In a simple model, a horizontal flip could be thought to reverse direction,
        // but we'll stick to the principle that flip affects only mirroring, not rotation here,
        // since we're applying flips directly in the absolute state.
    }

    // Ensure the rotation stays within the 0-360 range.
    absolute.rotation = fmod(absolute.rotation, 360.0f);
    if (absolute.rotation < 0) {
        absolute.rotation += 360.0f;
    }

    return absolute;
}