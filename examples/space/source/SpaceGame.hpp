#pragma once

#include "BlinkGame.hpp"
#include "LayeredRenderer.hpp"

class SpaceGame : public BlinkGame
{
public:
    SpaceGame() = default;
    ~SpaceGame() = default;
    void init() override;
    void run() override;

    void handleEvent(const SDL_Event& event) override;
    void render() override;
    void clear() override;
    void applyResolution(int, int) override;
    
    SDL_Texture* spaceBase;
    SDL_Texture* spaceFull;
    SDL_Rect dest;
    SDL_Rect clip;
    int color = 0;

    int mode = 0;
    SDL_Renderer* renderer;

    int isMouseDown = 0;
    int startX = 0, startY = 0;
    int currentX = 0, currentY = 0;
    float distance = 0.0f;
    float scale = 1;
    int tsize = 256;
    LayeredRenderer lrender1;
    SDL_Point internalUnits = {0,0};
};