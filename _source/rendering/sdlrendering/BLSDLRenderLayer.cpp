#include "BLSDLRenderLayer.hpp"

void BLSDLRenderLayer::addToLayer(std::function<void()> renderLambda)
{
    renderLambdas.push_back(renderLambda);
}

void BLSDLRenderLayer::render()
{
    for (auto& renderLambda : renderLambdas)
    {
        renderLambda();
    }    
}

void BLSDLRenderLayer::clear()
{
    renderLambdas.clear();
}