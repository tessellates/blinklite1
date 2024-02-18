
#include "AccMovementComponent.hpp"
#include "Transform2D.hpp"
#include "BLApplication.hpp"

#include <cmath>

AccMovementComponent::AccMovementComponent(const Vec2& startPosition, const Vec2& targetPosition, float duration) : startPosition(startPosition), targetPosition(targetPosition), animationDuration(duration)
{

}

bool AccMovementComponent::update(Node& node)
{
    if (arrived)
        return false;

    elapsedTime += BLApplication::deltaTime;
    float fraction = elapsedTime / animationDuration;

    // Apply acceleration factor to the fraction
    float acceleration = 3.0f; // Adjust this value to control the acceleration

    fraction = pow(fraction, acceleration);

    if (fraction >= 1) {
        fraction = 1;
        arrived = true;
    }


    node.relative.position = startPosition + (targetPosition - startPosition) * fraction;
    node.updatePosition();
    return true;
}