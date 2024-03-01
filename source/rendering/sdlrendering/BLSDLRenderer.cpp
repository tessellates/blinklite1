#include <iostream>

#include "BLSDLRenderer.hpp"


BLSDLRenderer::BLSDLRenderer(SDL_Renderer* renderer) : sdlRenderer(renderer) {}

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

    // Calculate the rotation center
    SDL_Point center = {info.dest.w / 2, info.dest.h / 2};

    const SDL_Rect* clip = &info.clip;
    if (info.clip.h == 0 || info.clip.w == 0)
    {
        clip = nullptr;
    }
    // Perform the actual rendering
    SDL_RenderCopyEx(sdlRenderer, texture, clip, &info.dest, info.rotation, &center, info.flip);
}
