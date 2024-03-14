#pragma once

#include "BLSDLRenderLayer.hpp"
#include "BLSDLRenderTextureLayer.hpp"
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
    void clear();
    void toggleTextureLayerMode(bool enable);
    void addRenderTarget(const RenderInfo& info);
    void renderInfo(const RenderInfo& info, bool debug = false);

    void applyResolution(int xResolution, int yResolution);
    void setInternalUnits(const SDL_Point&);
    void setFrameLayout(const FrameLayout&, int, int);

    Vec2 pointInUnits(const SDL_Point& point);

public:
    std::array<BLSDLRenderLayer, 10> layers;
    SDL_Renderer* renderer;
    BLSDLTextureManager textureManager;
    SDL_Color backgroundColor = {0, 0, 0, 255};

public:
    void updateContext();

    FrameLayout frameLayout;
    SDL_Rect absoluteLayout;
    SDL_Point internalUnits;

    float xScale = 1;
    float yScale = 1;

    SDL_Texture* target = NULL;
};
