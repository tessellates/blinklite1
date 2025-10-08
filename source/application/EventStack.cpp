#include "EventStack.hpp"

void EventStack::emit_axes()
{
    const float x = (holdR?1.f:0.f) - (holdL?1.f:0.f);
    const float y = (holdD?1.f:0.f) - (holdU?1.f:0.f);
    out.push_back({Event::MoveX, push(F1{ x })});
    out.push_back({Event::MoveY, push(F1{ y })});
}

void EventStack::finalize_frame()
{
    const float x = (holdR?1.f:0.f) - (holdL?1.f:0.f);
    const float y = (holdD?1.f:0.f) - (holdU?1.f:0.f);
    out.push_back({Event::MoveX, push(F1{ x })});
    out.push_back({Event::MoveY, push(F1{ y })});
}
// call once per frame after draining SDL events
const std::vector<EventData>& EventStack::flush()
{
    finalize_frame();
    return out; // consumer reads, then calls reset()
}

// clear for next frame (must be called after consumer is done with pointers) 
void EventStack::reset()
{
    out.clear();
    arena.clear();
}