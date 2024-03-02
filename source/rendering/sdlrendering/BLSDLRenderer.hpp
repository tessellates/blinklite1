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
    BLSDLRenderer();
    BLSDLRenderer(SDL_Renderer* renderer);

    void render();
    void addRenderTarget(const RenderInfo& info);
    void renderInfo(const RenderInfo& info);

    void setInternalContext(const SDL_Rect& context);
    void setExternalContext(const SDL_Point& context);

    const SDL_Point& externals() const;
    const SDL_Rect& internals() const;

public:
    std::array<BLSDLRenderLayer, 10> layers;
    SDL_Renderer* sdlRenderer;
    BLSDLTextureManager textureManager;

private:
    void updateContext();

    SDL_Rect internalContext;
    SDL_Point externalContext;

    float xScale = 1;
    float yScale = 1;
};
