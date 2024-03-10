#pragma once

#include <SDL.h>
#include "RenderInfo.hpp"

class BLSDLRenderTextureLayer
{
public:
    BLSDLRenderTextureLayer() = default;
    void addToLayer(std::function<void()> renderLambda);
    void render();
    void clear();
    void initTexture(int x, int y);
    void destroyTexture();

public:
    std::vector<std::function<void()>> renderLambdas;
    SDL_Texture* layerTexture = nullptr;
    SDL_Renderer* renderer;
};