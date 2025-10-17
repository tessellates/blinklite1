#pragma once
#include "ConnectModel.hpp"
#include "ConnectGui.hpp"
#include <BaseModule.hpp>
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
    void renderBoard(RenderSnapshot2D& snapshot);
    void renderUI(RenderSnapshot2D& snapshot);
    void updateGameVisuals();
    void makeMove(int column);
    void resetGame();
    void checkWinCondition();
    void syncGuiWithModel();
    
    // Helper to add rectangle as QuadCmd
    void addRectQuad(RenderSnapshot2D& snapshot, glm::vec4 rect, glm::vec4 color);
    
    glm::vec2 getBoardPosition() const;
    glm::vec4 getPlayerColor(ConnectModel::Player player) const;
    
private:
    ConnectModel model;
    std::unique_ptr<ConnectGui> gui;
    Grid gameGrid;
    TextureHandle whitePixelTexture;
    
    int hoveredColumn = -1;
    bool gameOver = false;
    ConnectModel::Player winner = ConnectModel::Player::None;
    
    static constexpr float BOARD_MARGIN = 50.0f;
    static constexpr float CELL_SIZE = 60.0f;
    static constexpr glm::vec4 BOARD_COLOR = {0.2f, 0.4f, 0.8f, 1.0f};
    static constexpr glm::vec4 EMPTY_CELL_COLOR = {0.9f, 0.9f, 0.9f, 1.0f};
    static constexpr glm::vec4 HOVER_COLOR = {0.8f, 0.8f, 0.8f, 0.5f};
};
