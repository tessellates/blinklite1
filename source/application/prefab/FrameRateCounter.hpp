#pragma once

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <string>

#include "BaseModule.hpp"

class FrameRateCounter : public BaseModule
{
public:
    FrameRateCounter() = default;
    FrameRateCounter(SDL_Renderer* renderer, TTF_Font* font); 
    void update(); 
    void render(); 

    void tick(float dt) override;
    void extract( RenderSnapshots2D& s) override;

private:
    SDL_Renderer* renderer;
    TTF_Font* font;
    SDL_Texture* textTexture = nullptr;
    SDL_Color textColor = { 120, 0, 120, 255 };
    Uint32 prevTime = SDL_GetTicks();
    Uint32 frames = 0;
    float fps = 0;
};
