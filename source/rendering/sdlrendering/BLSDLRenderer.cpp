#include <iostream>

#include "BLSDLRenderer.hpp"
#include "SDLUtil.hpp"
#include "SDLPrints.hpp"
#include "BLApplication.hpp"


BLSDLRenderer::BLSDLRenderer()
{
    internalUnits = {1152, 1152};
    updateContext();
}

BLSDLRenderer::BLSDLRenderer(SDL_Renderer* renderer) : BLSDLRenderer()
{
    renderer = renderer;
}

void BLSDLRenderer::applyResolution(int xResolution, int yResolution)
{
    absoluteLayout.h = yResolution*frameLayout.ySize;
    absoluteLayout.w = absoluteLayout.h*frameLayout.xyRatio;
    absoluteLayout.y = -absoluteLayout.h/2+yResolution*frameLayout.y;
    absoluteLayout.x = -absoluteLayout.w/2+xResolution*frameLayout.x;
    updateContext();
    resetTextures();
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
    SDL_SetRenderDrawColor(renderer, backgroundColor.r, backgroundColor.b, backgroundColor.g, backgroundColor.a); // Color #deebd4
    //SDL_SetRenderDrawColor(renderer, 0,0,0,255); // Color #deebd4
    SDL_RenderFillRect(renderer, &absoluteLayout);
    if (textureLayerMode)
    {
        for (auto& layer : textureLayers)
        {
            layer.render();
        }
        SDL_SetRenderTarget(renderer, NULL);

        for (auto& layer : textureLayers)
        {
            SDL_Rect clip = absoluteLayout;

            clip.x = 0;
            clip.y = 0;
            SDL_RenderCopy(renderer, layer.layerTexture, nullptr, &absoluteLayout);
        }
    }
    else
    {
        for (auto& layer : layers)
        {
            layer.render();
        }

    }
}

void BLSDLRenderer::clear()
{
    for (auto& layer : layers)
    {
        layer.clear();
    }
    if (textureLayerMode)
    {
        for (auto& layer : textureLayers)
        {
            layer.clear();
        }  
    }

}

void BLSDLRenderer::addRenderTarget(const RenderInfo& info)
{
    if (textureLayerMode)
    {
        textureLayers[info.layerID].addToLayer([this, info]() {this->renderInfo(info);});
    }
    else
    {
        layers[info.layerID].addToLayer([this, info]() {this->renderInfo(info);});
    }
}

void BLSDLRenderer::renderInfo(const RenderInfo& info, bool debug)
{
    if (!info.visible) return; // Skip rendering if not visible

    SDL_Texture* texture = textureManager.getTexture(info.textureID);
    if (textureLayerMode)
    {
        SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_NONE);
    }
    if (texture == nullptr) {
        std::cout << "textureID:" << info.textureID << " not found" << std::endl;
        texture = textureManager.getTexture(0);
    }

    auto destf = info.dest;
    if(textureLayerMode)
    {
        multiply_rect_x(destf, txScale);
        multiply_rect_y(destf, tyScale);
    }
    else
    {
        multiply_rect_x(destf, xScale);
        multiply_rect_y(destf, yScale);
    }
    auto dest = cast_rect_smart(destf);
    if (!textureLayerMode)
    {
        dest.x += absoluteLayout.x;
        dest.y += absoluteLayout.y;
    }
    else
    {

    }

    if (debug)
    {
        std::cout << "-------------" << destf.x << std::endl;
        std::cout << "float:" << std::endl;
        std::cout << "x:" << destf.x << std::endl;
        std::cout << "y:" << destf.y << std::endl;
        std::cout << "w:" << destf.z << std::endl;
        std::cout << "h:" << destf.w << std::endl;
        std::cout << "int:" << std::endl;
        print(dest);
    }
    //dest.h = std::min(dest.h, absoluteLayout.h);
    //dest.w = std::min(dest.w, absoluteLayout.w);

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
            centerStack = {(int)info.center.x, (int)info.center.y};
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
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

    //SDL_RenderCopy(renderer, texture, clip, &dest);
}

Vec2 BLSDLRenderer::pointInUnits(const SDL_Point& point)
{
    if (SDL_PointInRect(&point, &absoluteLayout)) 
    {
        float x = float(point.x - absoluteLayout.x)/xScale;
        float y = float(point.y - absoluteLayout.y)/yScale;
        return {x,y};
    }
    else
    {
        return {-1, -1};
    }
}

void BLSDLRenderer::toggleTextureLayerMode(bool enable)
{
    if (!textureLayerMode && enable)
    {
        textureLayerMode = enable;
        resetTextures();
        std::cout << "LAYER IS ON" << std::endl;
        return;
    }
    else
    {
        std::cout << "LAYER IS OFF" << std::endl;
        textureLayerMode = enable;
    }
}

void BLSDLRenderer::resetTextures(int x, int y)
{
    auto dis = BLApplication::currentDisplay();
    if (textureLayerMode)
    {
        for (auto& layer : textureLayers)
        {
            layer.renderer = renderer;
            if (layer.layerTexture != nullptr)
            {
                layer.destroyTexture();
            }
            layer.initTexture(dis.h, dis.h);
        }
        std::cout << dis.w << std::endl;
        std::cout << dis.h << std::endl;
        std::cout << "test" << std::endl;
        std::cout << absoluteLayout.h << std::endl;
        std::cout << dis.h << std::endl;
        txScale = xScale/(absoluteLayout.w/dis.w);
        tyScale = yScale/(absoluteLayout.h/dis.h);
        txScale = tyScale;
        std::cout << txScale << std::endl;
        std::cout << tyScale << std::endl;
        std::cout << xScale << std::endl;
        std::cout << yScale << std::endl;
    }
}