#include "SDLUserEvents.hpp"

Uint32 BL_RESOLUTIONCHANGE = 0;
Uint32 BL_FULLSCREEN_TOGGLE = 0;

bool RegisterEngineEvents() {
    const Uint32 base = SDL_RegisterEvents(2);
    if (!base) return false;            // failed
    BL_RESOLUTIONCHANGE  = base + 0;
    BL_FULLSCREEN_TOGGLE = base + 1;
    return true;
}