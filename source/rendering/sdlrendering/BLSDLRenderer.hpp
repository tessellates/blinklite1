#pragma once

#include "BLSDLRenderLayer.hpp"
#include "RenderInfo.hpp"
#include "BLSDLTextureManager.hpp"
#include "FrameLayout.hpp"

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

    void applyResolution(int xResolution, int yResolution);
    void setInternalUnits(const SDL_Point&);
    void setFrameLayout(const FrameLayout&, int, int);

    //const SDL_Rect& getAbsoluteLayout() const;
    //const SDL_Point& getInternalUnits() const;
    //const FrameLayout& getFrameLayout() const;

public:
    std::array<BLSDLRenderLayer, 10> layers;
    SDL_Renderer* sdlRenderer;
    BLSDLTextureManager textureManager;

public:
    void updateContext();

    FrameLayout frameLayout;
    SDL_Rect absoluteLayout;
    SDL_Point internalUnits;

    float xScale = 1;
    float yScale = 1;
};
