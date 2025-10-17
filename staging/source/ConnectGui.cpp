/**
 * Copyright (C) ZdefaultCompanyZ - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */
#include "ConnectGui.hpp"
#include <algorithm>
#include <cmath>
#include <glm/ext/matrix_transform.hpp> 

ConnectGui::ConnectGui(Engine& engine) : engine(engine) {
    // No texture needed anymore
}

void ConnectGui::init(const Grid& grid) {
    this->grid = grid;
    pieces.clear();
}

void ConnectGui::update(double deltaTime) {
    updateAnimations(deltaTime);
}

void ConnectGui::render(RenderSnapshot2D& snapshot) {
    for (const auto& [coord, piece] : pieces) {
        renderPiece(snapshot, piece, coord);
    }
}

void ConnectGui::addConnectEntity(const Coordinate& position, int color) {
    PieceEntity piece;
    piece.color = color;
    piece.isPreview = previewMode;
    piece.targetPosition = gridToWorldPosition(position);
    
    if (previewMode) {
        piece.currentPosition = piece.targetPosition;
        piece.startPosition = piece.targetPosition;
        piece.isAnimating = false;
    } else {
        Coordinate dropStart(position.x, -2);
        piece.startPosition = gridToWorldPosition(dropStart);
        piece.currentPosition = piece.startPosition;
        piece.isAnimating = true;
        piece.animationTime = 0.0f;
        piece.animationDuration = 0.75f;
    }
    
    pieces[position] = piece;
}

void ConnectGui::removeConnectEntity(const Coordinate& position) {
    pieces.erase(position);
}

void ConnectGui::changeConnectEntity(const Coordinate& position, int color) {
    auto it = pieces.find(position);
    if (it != pieces.end()) {
        it->second.color = color;
        it->second.isPreview = previewMode;
    }
}

glm::vec2 ConnectGui::gridToWorldPosition(const Coordinate& coord) const {
    return grid.point(coord);
}

glm::vec4 ConnectGui::getPieceColor(int colorId, bool isPreview) const {
    int index = colorId;
    if (isPreview) {
        index += 3;
    }
    index = std::clamp(index, 0, 5);
    return COLORS[index];
}

void ConnectGui::updateAnimations(double deltaTime) {
    for (auto& [coord, piece] : pieces) {
        if (piece.isAnimating) {
            piece.animationTime += static_cast<float>(deltaTime);
            
            if (piece.animationTime >= piece.animationDuration) {
                piece.currentPosition = piece.targetPosition;
                piece.isAnimating = false;
                piece.animationTime = piece.animationDuration;
            } else {
                float t = piece.animationTime / piece.animationDuration;
                float easedT = 1.0f - (1.0f - t) * (1.0f - t);
                piece.currentPosition = glm::mix(piece.startPosition, piece.targetPosition, easedT);
            }
        }
    }
}

void ConnectGui::renderPiece(RenderSnapshot2D& snapshot, const PieceEntity& piece, const Coordinate& coord) {
    float cellSize = std::min(grid.xsize, grid.ysize);
    float radius = cellSize * PIECE_RADIUS_RATIO;
    
    glm::vec4 color = getPieceColor(piece.color, piece.isPreview);
    uint32_t sortKey = piece.isPreview ? 2 : 1;
    
    addCircleQuad(snapshot, piece.currentPosition, radius, color, sortKey);
}

void ConnectGui::addCircleQuad(RenderSnapshot2D& snapshot, glm::vec2 center, float radius, glm::vec4 color, uint32_t sortKey) {
    QuadCmd quad;
    quad.tex = 0; // No texture - solid color
    quad.color = color;
    quad.sortKey = sortKey;
    
    glm::mat4 transform = glm::mat4(1.0f);
    transform = glm::translate(transform, glm::vec3(center.x - radius, center.y - radius, 0.0f));
    transform = glm::scale(transform, glm::vec3(radius * 2.0f, radius * 2.0f, 1.0f));
    
    glm::mat4 projection = glm::mat4(1.0f);
    glm::mat4 view = glm::mat4(1.0f);
    
    quad.mvp = projection * view * transform;
    quad.uv = glm::vec4(0, 0, 1, 1);
    
    snapshot.quads.push_back(quad);
}