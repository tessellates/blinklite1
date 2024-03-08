#pragma once

#include <SDL.h>
#include <vector>

class BlinkGame
{
public:
    BlinkGame() = default;
    virtual ~BlinkGame() = default;
    virtual void init();
    virtual void run();
    virtual void handleEvent(const SDL_Event& event);

    std::vector<std::pair<int,int>> resolutions;
};
