/**
 * Copyright (C) ZdefaultCompanyZ - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */
#pragma once

#include <Coordinate.hpp>
#include <Grid.hpp>
#include <CoreTypes.hpp>
#include <Engine.hpp>
#include <glm/glm.hpp>
#include <unordered_map>
#include <memory>

// Forward declarations
class ShapeRenderer;

/**
 * ConnectGui - Handles visual representation of Connect4 pieces
 * Refactored to use the new BlinkLite engine architecture
 */
class ConnectGui {
public:
    struct PieceEntity {
        glm::vec2 currentPosition;
        glm::vec2 targetPosition;
        glm::vec2 startPosition;
        int color;
        bool isAnimating = false;
        float animationTime = 0.0f;
        float animationDuration = 0.75f; // 750ms like original
        bool isPreview = false;
    };

    explicit ConnectGui(Engine& engine);
    ~ConnectGui() = default;

    // Core functionality
    void init(const Grid& grid);
    void update(double deltaTime);
    void render(RenderSnapshot2D& snapshot);

    // Entity management
    void addConnectEntity(const Coordinate& position, int color);
    void removeConnectEntity(const Coordinate& position);
    void changeConnectEntity(const Coordinate& position, int color);

    // Preview mode for hover effects
    void setPreviewMode(bool enabled) { previewMode = enabled; }
    bool isPreviewMode() const { return previewMode; }

    // Animation control
    void setAnimationDuration(float duration) { defaultAnimationDuration = duration; }

private:
    // Helper methods
    glm::vec2 gridToWorldPosition(const Coordinate& coord) const;
    glm::vec4 getPieceColor(int colorId, bool isPreview) const;
    void updateAnimations(double deltaTime);
    void renderPiece(RenderSnapshot2D& snapshot, const PieceEntity& piece, const Coordinate& coord);

private:
    Engine& engine;
    Grid grid;
    
    // Entity storage - using Coordinate as key
    std::unordered_map<Coordinate, PieceEntity> pieces;
    
    // Rendering resources
    TextureHandle whitePixelTexture;
    std::unique_ptr<ShapeRenderer> shapeRenderer;
    
    // Settings
    bool previewMode = false;
    float defaultAnimationDuration = 0.75f;
    
    // Visual constants
    static constexpr float PIECE_RADIUS_RATIO = 0.4f; // Piece size relative to grid cell
    static constexpr int LAYER_NORMAL = 1;
    static constexpr int LAYER_PREVIEW = 2;
    
    // Colors matching your original system (1+color, +3 for preview)
    static constexpr glm::vec4 COLORS[6] = {
        {1.0f, 0.2f, 0.2f, 1.0f}, // Color 0 (Red)
        {1.0f, 1.0f, 0.2f, 1.0f}, // Color 1 (Yellow)  
        {0.2f, 0.2f, 1.0f, 1.0f}, // Color 2 (Blue)
        {1.0f, 0.4f, 0.4f, 0.7f}, // Color 0 Preview (Light Red)
        {1.0f, 1.0f, 0.4f, 0.7f}, // Color 1 Preview (Light Yellow)
        {0.4f, 0.4f, 1.0f, 0.7f}  // Color 2 Preview (Light Blue)
    };
};

// Hash specialization for Coordinate to work with unordered_map
namespace std {
    template<>
    struct hash<Coordinate> {
        size_t operator()(const Coordinate& coord) const {
            return hash<int>()(coord.x) ^ (hash<int>()(coord.y) << 1);
        }
    };
}


