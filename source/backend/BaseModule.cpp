#include "BaseModule.hpp"
#include "Engine.hpp"
#include "EventStack.hpp"
#include "CoreTypes.hpp"

void BaseModule::onEvent(const EventData& event)
{
    // default: do nothing
}

void BaseModule::tick(float dt) 
{
    // default: do nothing
}
void BaseModule::extract( RenderSnapshots2D& s)
{
    // default: do nothing
}
void BaseModule::initialize()
{
    this->isInitialized = true;
    this->onInit();
}
void BaseModule::onInit()
{
    // default: do nothing
}