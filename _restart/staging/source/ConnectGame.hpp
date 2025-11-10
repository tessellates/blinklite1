#pragma once
#include "ConnectModel.hpp"
#include "ConnectGui.hpp"
#include <BaseModule.hpp>
#include <Event.hpp>
#include <Engine.hpp>
#include <CoreTypes.hpp>
#include <Grid.hpp>
#include <glm/glm.hpp>
#include <memory>

class ConnectGame : public BaseModule {
public:
    ConnectGame();
    ~ConnectGame() override = default;

    void tick(float dt, std::span<const EventData> ev) override;
    void extract(RenderSnapshots2D& snapshots) override;
    void onInit() override;

private:
    void setupBoard();
    void setupGrid();
    void handleInput(const EventData& event);
    void updateGameVisuals();
    void makeMove(int column);
    void hover(int column);
    int getColumnFromMouseX(float mouseX) const;
    void resetGame();
    
    glm::vec2 getBoardPosition() const;
    glm::vec4 getPlayerColor(int player) const;
    
private:
    ConnectModel model;
    std::unique_ptr<ConnectGui> gui;
    Grid gameGrid;
    
    int hoveredColumn = -1;
    
    static constexpr float BOARD_MARGIN = 50.0f;
    static constexpr float CELL_SIZE = 60.0f;
    static constexpr glm::vec4 BOARD_COLOR = {0.2f, 0.4f, 0.8f, 1.0f};
    static constexpr glm::vec4 EMPTY_CELL_COLOR = {0.9f, 0.9f, 0.9f, 1.0f};
    static constexpr glm::vec4 HOVER_COLOR = {0.8f, 0.8f, 0.8f, 0.5f};
};
