#include <iostream>

#include "LayeredRenderer.hpp"
#include "SDLUtil.hpp"
#include "SDLPrints.hpp"
#include "BLApplication.hpp"

LayeredRenderer::~LayeredRenderer()
{
    SDL_DestroyTexture(target);
}

void LayeredRenderer::render()
{
    SDL_SetRenderTarget(renderer, target);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
    SDL_SetRenderDrawColor(renderer, backgroundColor.r, backgroundColor.b, backgroundColor.g, backgroundColor.a); // Color #deebd4
    SDL_RenderFillRect(renderer, &context.absoluteLayout);
    
    for (auto& layer : layers)
    {
        layer.render();
    }
    
    SDL_SetRenderTarget(renderer, NULL);
}

void LayeredRenderer::clear()
{
    for (auto& layer : layers)
    {
        layer.clear();
    }
}

void LayeredRenderer::addRenderTarget(const RenderInfo& info)
{
    layers[info.layerID].addToLayer([this, info]() {this->renderInfo(info);});
}

void LayeredRenderer::renderInfo(const RenderInfo& info, bool debug)
{
    if (!info.visible) return; // Skip rendering if not visible

    SDL_Texture* texture = textureManager.getTexture(info.textureID);

    if (texture == nullptr) {
        texture = textureManager.getTexture(0);
    }

    auto dest = context.internalToAbsolute(info.dest);
   
    // Calculate the rotation center
    SDL_Point* center = nullptr;
    SDL_Point centerStack;

    if (info.rotation != 0)
    {
        if (!info.useCenter)
        {
            // resort to default center;
            int size = std::min(dest.h, dest.w);
            centerStack = {size/2, size/2};
        }
        else
        {
            centerStack = context.internalToAbsolute(info.center);
        }
        center = &centerStack;
    }

    const SDL_Rect* clip = &info.clip;
    if (info.clip.h == 0 || info.clip.w == 0)
    {
        clip = nullptr;
    }
    // Perform the actual rendering
    SDL_RenderCopyEx(renderer, texture, clip, &dest, info.rotation, center, info.flip);
}
