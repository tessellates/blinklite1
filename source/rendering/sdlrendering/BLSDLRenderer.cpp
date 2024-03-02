#include <iostream>

#include "BLSDLRenderer.hpp"
#include "SDLUtil.hpp"


BLSDLRenderer::BLSDLRenderer()
{
    internalUnits = {2000, 2000};
    updateContext();
}

BLSDLRenderer::BLSDLRenderer(SDL_Renderer* renderer) : BLSDLRenderer()
{
    sdlRenderer = renderer;
}

void BLSDLRenderer::applyResolution(int xResolution, int yResolution)
{
    absoluteLayout.h = yResolution*frameLayout.ySize;
    absoluteLayout.w = absoluteLayout.h*frameLayout.xyRatio;
    absoluteLayout.y = -absoluteLayout.h/2+yResolution*frameLayout.y;
    absoluteLayout.x = -absoluteLayout.w/2+xResolution*frameLayout.x;
    updateContext();
}

void BLSDLRenderer::setInternalUnits(const SDL_Point& units)
{
    internalUnits = units;
    updateContext();
}

void BLSDLRenderer::setFrameLayout(const FrameLayout& layout, int xResolution, int yResolution)
{
    frameLayout = layout;
    applyResolution(xResolution, yResolution);
}

void BLSDLRenderer::updateContext()
{
    xScale = (float)absoluteLayout.w/(float)internalUnits.x;
    yScale = (float)absoluteLayout.h/(float)internalUnits.y;
}

void BLSDLRenderer::render()
{
    for (auto& layer : layers)
    {
        layer.render();
        layer.clear();
    }
}

void BLSDLRenderer::addRenderTarget(const RenderInfo& info)
{
    layers[info.layerID].addToLayer([this, &info]() {this->renderInfo(info);});
}

void BLSDLRenderer::renderInfo(const RenderInfo& info)
{
    if (!info.visible) return; // Skip rendering if not visible

    SDL_Texture* texture = textureManager.getTexture(info.textureID);
    if (texture == nullptr) {
        std::cout << "textureID:" << info.textureID << " not found" << std::endl;
        // Handle cases where texture is not found, such as drawing a placeholder or logging an error
        return;
    }

    SDL_Rect dest = info.dest;
    multiply_rect_x(dest, xScale);
    multiply_rect_y(dest, yScale);
    dest.x += absoluteLayout.x;
    dest.y += absoluteLayout.y;

    //dest.h = std::min(dest.h, absoluteLayout.h);
    //dest.w = std::min(dest.w, absoluteLayout.w);

    // Calculate the rotation center
    SDL_Point center = {dest.w / 2, dest.h / 2};

    const SDL_Rect* clip = &info.clip;
    if (info.clip.h == 0 || info.clip.w == 0)
    {
        clip = nullptr;
    }
    // Perform the actual rendering
    SDL_RenderCopyEx(sdlRenderer, texture, clip, &dest, info.rotation, &center, info.flip);
}
