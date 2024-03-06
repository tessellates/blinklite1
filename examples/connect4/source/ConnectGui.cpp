/**
 * Copyright (C) ZdefaultCompanyZ - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */
#include "ConnectGui.hpp"
#include "RenderInfo.hpp"
#include "SDLPrints.hpp"

void ConnectGui::init(const Grid& grid)
{
    this->grid = grid;
}

void ConnectGui::addConnectEntity(const Coordinate& position, int color)
{
    int textureID = 1+color;
    if (previewMode)
    {
        textureID += 3;
    }

    Node node;
    node.relative.position = grid.point(position);
    node.updatePosition();
    TextureInfo info;
    info.dest = {0, 0, (int)grid.xsize, (int)grid.ysize};
    info.textureID = textureID;

    nodes.insert({position, {node, info}});
}

void ConnectGui::removeConnectEntity(const Coordinate& position)
{
    nodes.erase(position);
}

void ConnectGui::changeConnectEntity(const Coordinate& position, int color)
{
    nodes[position].second.textureID = 1 + color + 3*previewMode;
}

void ConnectGui::render()
{    
    for (auto& render : nodes)
    {
        auto x = createInfo(&render.second.first, &render.second.second);
        gameRenderer->addRenderTarget(x);
    }
}