/**
 * Copyright (C) ZdefaultCompanyZ - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */
#include "ConnectGui.hpp"
#include "RenderInfo.hpp"
#include "SDLPrints.hpp"
#include "MovementComponent.hpp"
#include "AccMovementComponent.hpp"

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

    Vec2 start = grid.point(Coordinate(position.x, -2));

    if (previewMode)
    {
        start = grid.point(position);
    }

    Node node;
    node.relative.position = start;
    node.updatePosition();
    TextureInfo info;
    info.dest = {0, 0, grid.xsize, grid.ysize};
    info.textureID = textureID;

    if (!previewMode)
    {
        if (true)
        {
            auto end = grid.point(position);
            auto movementComponent = AccMovementComponent(start, end, 500);
            Node::UpdateMethod movementLambda = [movementComponent](Node& node) mutable {
                return movementComponent.update(node);
            };
            node.addUpdateMethod(movementLambda);
        }
    }
    //node.addUpdateMethod([])
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
        render.second.first.update();
        auto x = createInfo(render.second.first, render.second.second);
        gameRenderer->addRenderTarget(x);
    }
}