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

    void resetTextures();
    //const SDL_Rect& getAbsoluteLayout() const;
    //const SDL_Point& getInternalUnits() const;
    //const FrameLayout& getFrameLayout() const;

public:
    std::array<BLSDLRenderLayer, 10> layers;
    std::array<BLSDLRenderTextureLayer, 10> textureLayers;
    SDL_Renderer* renderer;
    BLSDLTextureManager textureManager;
    SDL_Color backgroundColor = {222, 235, 212, 255};

public:
    void updateContext();

    FrameLayout frameLayout;
    SDL_Rect absoluteLayout;
    SDL_Point internalUnits;

    float xScale = 1;
    float yScale = 1;

    float txScale = 1;
    float tyScale = 1;

    bool textureLayerMode = false;
};
