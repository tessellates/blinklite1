/**
 * Copyright (C) ZdefaultCompanyZ - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */
#include "ConnectGui.hpp"
#include <algorithm>
#include <cmath>
#include <glm/ext/matrix_transform.hpp> 

ConnectGui::ConnectGui() {
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
    piece.targetPosition = grid.point(position);
    
    if (previewMode) {
        piece.currentPosition = piece.targetPosition;
        piece.startPosition = piece.targetPosition;
        piece.isAnimating = false;
    } else {
        Coordinate dropStart(position.x, -2);
        piece.startPosition = grid.point(dropStart);
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
    
    addRectQuad(snapshot, piece.currentPosition, radius, color);
}

void ConnectGui::addRectQuad(RenderSnapshot2D& snapshot, glm::vec4 rect, glm::vec4 color) {
    QuadCmd quad;
    quad.tex = 0; // No texture - solid color
    quad.color = color;
    quad.sortKey = 0;
    
    glm::mat4 transform = glm::mat4(1.0f);
    transform = glm::translate(transform, glm::vec3(rect.x, rect.y, 0.0f));
    transform = glm::scale(transform, glm::vec3(rect.z, rect.w, 1.0f));
    
    glm::mat4 projection = glm::mat4(1.0f);
    glm::mat4 view = glm::mat4(1.0f);
    
    quad.mvp = projection * view * transform;
    quad.uv = glm::vec4(0, 0, 1, 1);
    
    snapshot.quads.push_back(quad);
}

void ConnectGui::renderUI(RenderSnapshot2D& snapshot) {
    glm::vec4 statusRect = {boardPos.x, boardPos.y - 40, 300, 30};
    
    glm::vec4 statusColor;
    if (model.gameOver) {
        statusColor = model.win ? getPlayerColor(!model.player) : glm::vec4{0.5f, 0.5f, 0.5f, 0.8f};
        statusColor.a = 0.8f;
    } else {
        statusColor = getPlayerColor(model.player);
        statusColor.a = 0.6f;
    }
    
    addRectQuad(snapshot, statusRect, statusColor);
}

glm::vec4 ConnectGui::getPlayerColor(int player) const {
    switch (player) {
        case 0: return {1.0f, 0.2f, 0.2f, 1.0f}; // Red
        case 1: return {1.0f, 1.0f, 0.2f, 1.0f}; // Yellow
        default: return {1.0f, 1.0f, 1.0f, 1.0f};
    }
}

void ConnectGame::renderBoard(RenderSnapshot2D& snapshot) 
{
    glm::vec2 boardPos = getBoardPosition();
    glm::vec2 boardSize = {ConnectModel::COLS * grid.csize, ConnectModel::ROWS * grid.csize};

    // Board background
    addRectQuad(snapshot, {boardPos.x - 10, boardPos.y - 10, boardSize.x + 20, boardSize.y + 20}, BOARD_COLOR);

    // Grid cells
    for (int row = 0; row < ConnectModel::ROWS; ++row) {
        for (int col = 0; col < ConnectModel::COLS; ++col) {
            glm::vec2 cellPos = {boardPos.x + col * grid.csize, boardPos.y + row * grid.csize};
            glm::vec4 cellRect = {cellPos.x + 1, cellPos.y + 1, grid.csize - 2, grid.csize - 2};
            
            glm::vec4 cellColor = EMPTY_CELL_COLOR;
            if (col == hoveredColumn && !model.gameOver) {
                cellColor = HOVER_COLOR;
            }
            
            addRectQuad(snapshot, cellRect, cellColor);
        }
    }
}
