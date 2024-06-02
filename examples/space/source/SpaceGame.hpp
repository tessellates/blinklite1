#pragma once

#include "BlinkGame.hpp"
#include "LayeredRenderer.hpp"
#include "TextureInfo.hpp"
#include "Node.hpp"

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
    int tsize = 1024;
    LayeredRenderer lrender1;
    SDL_Point internalUnits = {0,0};

    LayeredRenderer entityRenderer1;
    Node playerNode;
    TextureInfo playerTexture;
    Vec2 velocity = {0,0};
    Vec2 accel = {0,0};
    int keydown = 0;
    float speed = 0.1;
    BlinkContext mid;
    BlinkContext midline;
    float shiftmod = 4;
};