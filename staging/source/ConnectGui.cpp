/**
 * Copyright (C) ZdefaultCompanyZ - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */
#include "ConnectGui.hpp"
#include <ShapeRenderer.hpp>
#include <algorithm>
#include <cmath>

ConnectGui::ConnectGui(Engine& engine) : engine(engine) {
    // Initialize rendering resources
    whitePixelTexture = engine.createWhitePixelTexture();
    shapeRenderer = std::make_unique<ShapeRenderer>(engine, whitePixelTexture);
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
        // Preview pieces appear directly at target
        piece.currentPosition = piece.targetPosition;
        piece.startPosition = piece.targetPosition;
        piece.isAnimating = false;
    } else {
        // Normal pieces drop from above (like original: y = -2)
        Coordinate dropStart(position.x, -2);
        piece.startPosition = gridToWorldPosition(dropStart);
        piece.currentPosition = piece.startPosition;
        piece.isAnimating = true;
        piece.animationTime = 0.0f;
        piece.animationDuration = defaultAnimationDuration;
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
    // Convert grid coordinate to world position using your Grid class
    return grid.point(coord);
}

glm::vec4 ConnectGui::getPieceColor(int colorId, bool isPreview) const {
    // Map to your original texture ID system: 1+color, +3 for preview
    int index = colorId;
    if (isPreview) {
        index += 3; // Preview colors are offset by 3
    }
    
    // Clamp to valid range
    index = std::clamp(index, 0, 5);
    return COLORS[index];
}

void ConnectGui::updateAnimations(double deltaTime) {
    for (auto& [coord, piece] : pieces) {
        if (piece.isAnimating) {
            piece.animationTime += static_cast<float>(deltaTime);
            
            if (piece.animationTime >= piece.animationDuration) {
                // Animation complete
                piece.currentPosition = piece.targetPosition;
                piece.isAnimating = false;
                piece.animationTime = piece.animationDuration;
            } else {
                // Interpolate position - using accelerated movement like AccMovementComponent
                float t = piece.animationTime / piece.animationDuration;
                
                // Easing function - quadratic ease-out for natural fall
                float easedT = 1.0f - (1.0f - t) * (1.0f - t);
                
                piece.currentPosition = glm::mix(piece.startPosition, piece.targetPosition, easedT);
            }
        }
    }
}

void ConnectGui::renderPiece(RenderSnapshot2D& snapshot, const PieceEntity& piece, const Coordinate& coord) {
    // Calculate piece radius based on grid cell size
    float cellSize = std::min(grid.xsize, grid.ysize);
    float radius = cellSize * PIECE_RADIUS_RATIO;
    
    // Create circle at current position
    Circle circle;
    circle.center = piece.currentPosition;
    circle.radius = radius;
    
    // Get color based on piece properties
    glm::vec4 color = getPieceColor(piece.color, piece.isPreview);
    
    // Add to render snapshot with appropriate layer
    uint32_t sortKey = piece.isPreview ? LAYER_PREVIEW : LAYER_NORMAL;
    shapeRenderer->addCircle(snapshot, circle, color, sortKey);
}