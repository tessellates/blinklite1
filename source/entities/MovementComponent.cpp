
#include "MovementComponent.hpp"
#include "Transform2D.hpp"
#include "BLApplication.hpp"

MovementComponent::MovementComponent(const Vec2& startPosition, const Vec2& targetPosition, float duration) : startPosition(startPosition), targetPosition(targetPosition), animationDuration(duration)
{

}

bool MovementComponent::update(Node& node)
{
    if (arrived)
        return false;
    elapsedTime += BLApplication::deltaTime;
    float fraction = elapsedTime/animationDuration;
    if (fraction >= 1)
    {
        fraction = 1;
        arrived = true;
    }
    node.relative.position = startPosition + (targetPosition - startPosition)*fraction;
    node.updatePosition();
    return true;
}