#include "BaseModule.hpp"
#include "Engine.hpp"
#include "EventStack.hpp"
#include "CoreTypes.hpp"


void BaseModule::tick(float dt, std::span<const EventData> ev) 
{
    // default: do nothing
}
void BaseModule::extract( RenderSnapshots2D& s)
{
    // default: do nothing
}
void BaseModule::initialize(Engine* engine, EventStack* eventStack)
{
    this->engine = engine;
    this->eventStack = eventStack;
    this->isInitialized = true;
    this->onInit();
}
void BaseModule::onInit()
{
    // default: do nothing
}