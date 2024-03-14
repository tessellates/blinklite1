#include "BLSDLRenderTextureLayer.hpp"

void BLSDLRenderTextureLayer::addToLayer(std::function<void()> renderLambda)
{
    renderLambdas.push_back(renderLambda);
}

void BLSDLRenderTextureLayer::render()
{
    if (layerTexture == NULL)
    {
        return;
    }
    SDL_SetRenderTarget(renderer, layerTexture);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);
    for (auto& renderLambda : renderLambdas)
    {
        renderLambda();
    }    
    SDL_SetTextureBlendMode(layerTexture, SDL_BLENDMODE_BLEND);


}

void BLSDLRenderTextureLayer::clear()
{
    renderLambdas.clear();
}

void BLSDLRenderTextureLayer::initTexture(int x, int y)
{
    layerTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, x, y);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
    SDL_SetTextureBlendMode(layerTexture, SDL_BLENDMODE_BLEND);

    // Set the texture as the current rendering target
    SDL_SetRenderTarget(renderer, layerTexture);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);
    SDL_SetRenderTarget(renderer, NULL);
}

void BLSDLRenderTextureLayer::destroyTexture()
{
    SDL_DestroyTexture(layerTexture);
    layerTexture = nullptr;
}