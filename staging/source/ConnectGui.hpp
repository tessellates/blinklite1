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

class ConnectGui {
public:
    struct PieceEntity {
        glm::vec2 currentPosition;
        glm::vec2 targetPosition;
        glm::vec2 startPosition;
        int color;
        bool isAnimating = false;
        float animationTime = 0.0f;
        float animationDuration = 0.75f;
        bool isPreview = false;
    };

    explicit ConnectGui(Engine& engine);
    ~ConnectGui() = default;

    void init(const Grid& grid);
    void update(double deltaTime);
    void render(RenderSnapshot2D& snapshot);

    void addConnectEntity(const Coordinate& position, int color);
    void removeConnectEntity(const Coordinate& position);
    void changeConnectEntity(const Coordinate& position, int color);

    void setPreviewMode(bool enabled) { previewMode = enabled; }
    bool isPreviewMode() const { return previewMode; }

private:
    glm::vec2 gridToWorldPosition(const Coordinate& coord) const;
    glm::vec4 getPieceColor(int colorId, bool isPreview) const;
    void updateAnimations(double deltaTime);
    void renderPiece(RenderSnapshot2D& snapshot, const PieceEntity& piece, const Coordinate& coord);
    void addCircleQuad(RenderSnapshot2D& snapshot, glm::vec2 center, float radius, glm::vec4 color, uint32_t sortKey);

private:
    Engine& engine;
    Grid grid;
    std::unordered_map<Coordinate, PieceEntity> pieces;
    bool previewMode = false;
    
    static constexpr float PIECE_RADIUS_RATIO = 0.4f;
    static constexpr glm::vec4 COLORS[6] = {
        {1.0f, 0.2f, 0.2f, 1.0f}, // Red
        {1.0f, 1.0f, 0.2f, 1.0f}, // Yellow  
        {0.2f, 0.2f, 1.0f, 1.0f}, // Blue
        {1.0f, 0.4f, 0.4f, 0.7f}, // Red Preview
        {1.0f, 1.0f, 0.4f, 0.7f}, // Yellow Preview
        {0.4f, 0.4f, 1.0f, 0.7f}  // Blue Preview
    };
};


