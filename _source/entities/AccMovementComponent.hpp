#pragma once

#include "Node.hpp"

class AccMovementComponent {
public:
    AccMovementComponent(const Vec2& startPosition, const Vec2& targetPosition, float duration);
    bool update(Node& node);
private:
    Vec2 startPosition;
    Vec2 targetPosition;
    float animationDuration;
    float elapsedTime;
    bool arrived = false;
};