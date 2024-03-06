/**
 * Copyright (C) ZdefaultCompanyZ - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */
 #pragma once

#include <vector>
#include <unordered_map>

#include "Node.hpp"
#include "TextureInfo.hpp"
#include "Grid.hpp"
#include "BLSDLRenderer.hpp"

class ConnectGui
{
public:
    ConnectGui() = default;    // Constructor
    ~ConnectGui() = default;   // Destructor
    
    void init(const Grid& grid);
    void addConnectEntity(const Coordinate& postion, int color);
    void removeConnectEntity(const Coordinate& postion);
    void changeConnectEntity(const Coordinate& postion, int color);

    void render();
    
    std::unordered_map<Coordinate, std::pair<Node,TextureInfo>> nodes;

    Grid grid;

    BLSDLRenderer* gameRenderer;
    bool previewMode = false;
};


