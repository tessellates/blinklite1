#include <iostream>

#include "BLSDLRenderer.hpp"
#include "SDLUtil.hpp"


BLSDLRenderer::BLSDLRenderer() : externalContext({1440, 900}), internalContext({0,0,1440,900})
{
    updateContext();
}

BLSDLRenderer::BLSDLRenderer(SDL_Renderer* renderer) : BLSDLRenderer()
{
    sdlRenderer = renderer;
}

void BLSDLRenderer::setInternalContext(const SDL_Rect& internal)
{
    internalContext = internal;
    updateContext();
}

void BLSDLRenderer::setExternalContext(const SDL_Point& external)
{
    externalContext = external;
    updateContext();
}

void BLSDLRenderer::updateContext()
{
    xScale = (float)internalContext.w/(float)externalContext.x;
    yScale = (float)internalContext.h/(float)externalContext.y;
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
    dest.x += internalContext.x;
    dest.y += internalContext.y;

    dest.h = std::min(dest.h, internalContext.h);
    dest.w = std::min(dest.w, internalContext.w);

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

const SDL_Point& BLSDLRenderer::externals() const
{
    return externalContext;
}
const SDL_Rect& BLSDLRenderer::internals() const
{
    return internalContext;
}
