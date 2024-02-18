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
        // Handle cases where texture is not found, such as drawing a placeholder or logging an error
        return;
    }

    SDL_Rect destRect;
    destRect.x = static_cast<int>(info.x);
    destRect.y = static_cast<int>(info.y);
    destRect.w = static_cast<int>(info.width * info.scale); // Apply scale to width
    destRect.h = static_cast<int>(info.height * info.scale); // Apply scale to height

    // Calculate the rotation center
    SDL_Point center = {destRect.w / 2, destRect.h / 2};

    // Determine the flip flags
    SDL_RendererFlip flip = SDL_FLIP_NONE;
    if (info.flip == RenderInfo::Flip::Horizontal) {
        flip = SDL_FLIP_HORIZONTAL;
    } else if (info.flip == RenderInfo::Flip::Vertical) {
        flip = SDL_FLIP_VERTICAL;
    }

    // Perform the actual rendering
    SDL_RenderCopyEx(sdlRenderer, texture, nullptr, &destRect, info.rotation, &center, flip);
}
