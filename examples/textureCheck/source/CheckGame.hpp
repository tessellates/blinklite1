#pragma once

#include "BlinkGame.hpp"

class CheckGame : public BlinkGame
{
public:
    CheckGame() = default;
    ~CheckGame() = default;
    void init() override;
    void run() override;

    void handleEvent(const SDL_Event& event) override;
    void render() override;
    void clear() override;
    void applyResolution(int, int) override;
    
    SDL_Texture* test1;
    SDL_Texture* test2;
    SDL_Texture* test3;
    SDL_Texture* test4;
    SDL_Rect dest;

    int color = 0;

    int mode = 0;
    SDL_Renderer* renderer;
};