#pragma once

#include "BLSDLRenderLayer.hpp"
#include "RenderInfo.hpp"
#include "BLSDLTextureManager.hpp"
#include "BlinkContext.hpp"

#include <SDL.h>
#include <array>

class BLSDLTextureManager;
class RenderInfo;

class LayeredRenderer {
public:
    LayeredRenderer() = default;
    ~LayeredRenderer();
    void render();
    void clear();
    void addRenderTarget(const RenderInfo& info);
    void renderInfo(const RenderInfo& info, bool debug = false);

public:
    std::array<BLSDLRenderLayer, 10> layers;
    SDL_Renderer* renderer;
    BLSDLTextureManager textureManager;
    SDL_Color backgroundColor = {50, 50, 50, 255};

public:
    BlinkContext context;
    SDL_Texture* target = NULL;
};
