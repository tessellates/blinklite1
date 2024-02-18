#pragma once

#include "BLSDLRenderLayer.hpp"
#include "RenderInfo.hpp"
#include "BLSDLTextureManager.hpp"

#include <SDL.h>
#include <array>

class BLSDLTextureManager;
class RenderInfo;

class BLSDLRenderer {
public:
    BLSDLRenderer(SDL_Renderer* renderer);

    void render();
    void addRenderTarget(const RenderInfo& info);
    void renderInfo(const RenderInfo& info);

private:
    std::array<BLSDLRenderLayer, 10> layers;
    SDL_Renderer* sdlRenderer;
    BLSDLTextureManager textureManager;
};
    