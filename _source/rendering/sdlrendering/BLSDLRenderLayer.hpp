#pragma once

#include "RenderInfo.hpp"

#include <functional>
#include <vector>

class BLSDLRenderLayer
{
public:
    BLSDLRenderLayer() = default;
    void addToLayer(std::function<void()> renderLambda);
    void render();
    void clear();

private:
    std::vector<std::function<void()>> renderLambdas;
};