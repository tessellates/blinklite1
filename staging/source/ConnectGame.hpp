#pragma once
#include "ConnectModel.hpp"
#include <BaseModule.hpp>
#include <Engine.hpp>
#include <CoreTypes.hpp>
#include <glm/glm.hpp>

class ConnectGame : public BaseModule {
public:
    ConnectGame(Engine& engine);
    ~ConnectGame() override = default;

    void onInit() override;
    void onUpdate(double dt) override;
    void onRender(RenderSnapshot2D& snapshot) override;
    void onEvent(const EventData& event) override;
    void onCleanup() override;

private:
    void setupBoard();
    void handleInput(const EventData& event);
    void renderBoard(RenderSnapshot2D& snapshot);
    void renderPieces(RenderSnapshot2D& snapshot);
    void renderUI(RenderSnapshot2D& snapshot);
    void renderHoverPiece(RenderSnapshot2D& snapshot);
    
    // Game logic
    void makeMove(int column);
    void resetGame();
    void checkWinCondition();
    
    // Rendering helpers
    glm::vec2 getBoardPosition() const;
    glm::vec2 getCellSize() const;
    glm::vec2 getCellPosition(int row, int col) const;
    glm::vec4 getPlayerColor(ConnectModel::Player player) const;
    
private:
    Engine& engine;
    ConnectModel model;
    
    // Rendering resources
    TextureHandle boardTexture;
    TextureHandle redPieceTexture;
    TextureHandle yellowPieceTexture;
    TextureHandle whitePixelTexture;
    
    // Game state
    int hoveredColumn = -1;
    bool gameOver = false;
    ConnectModel::Player winner = ConnectModel::Player::None;
    
    // UI layout
    static constexpr float BOARD_MARGIN = 50.0f;
    static constexpr float CELL_SIZE = 60.0f;
    static constexpr float PIECE_RADIUS = 25.0f;
    static constexpr glm::vec4 BOARD_COLOR = {0.2f, 0.4f, 0.8f, 1.0f};
    static constexpr glm::vec4 EMPTY_CELL_COLOR = {0.9f, 0.9f, 0.9f, 1.0f};
    static constexpr glm::vec4 PLAYER1_COLOR = {1.0f, 0.2f, 0.2f, 1.0f}; // Red
    static constexpr glm::vec4 PLAYER2_COLOR = {1.0f, 1.0f, 0.2f, 1.0f}; // Yellow
    static constexpr glm::vec4 HOVER_COLOR = {0.8f, 0.8f, 0.8f, 0.5f};
};
