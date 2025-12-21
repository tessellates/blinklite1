#include "EventStack.hpp"

void EventStack::emit_axes()
{
    axes_emitted = true;
    const float x = (holdR?1.f:0.f) - (holdL?1.f:0.f);
    const float y = (holdD?1.f:0.f) - (holdU?1.f:0.f);
    
    if (x!=0.f)
        pushNow(Event::MoveX, F1{ x });
    if (y!=0.f) 
        pushNow(Event::MoveY, F1{ y });

}

void EventStack::finalize_frame()
{
    if (frameFinalized) return; // Only finalize once per frame
    
    if (axes_emitted)
    {
        axes_emitted = false;
        frameFinalized = true;
        return;
    }
    emit_axes();
    frameFinalized = true;
}

// NEW: Poll events one by one
bool EventStack::poll(EventData& event)
{
    if (!hasEvents()) 
    {      
        finalize_frame();
    }   
    if (pollIndex < out.size()) {
        event = out[pollIndex++];
        return true;
    }    
    return false;
}

bool EventStack::hasEvents()
{
    return pollIndex < out.size();
}


// Clear for next frame
void EventStack::reset()
{
    out = std::move(next);
    next.clear();
    currentArena.swap(nextArena);    
    // Reset polling state
    pollIndex = 0;
    frameFinalized = false;
    axes_emitted = false;
}