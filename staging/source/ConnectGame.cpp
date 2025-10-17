#include "ConnectGame.hpp"
#include "../../source/application/ShapeRenderer.hpp"
#include <iostream>

ConnectGame::ConnectGame(Engine& engine) : engine(engine) {}

void ConnectGame::onInit() {
    std::cout << "ConnectGame: Initializing..." << std::endl;
    
    // Load textures (using white pixel for shapes)
    whitePixelTexture = engine.createWhitePixelTexture();
    
    // Initialize game
    setupBoard();
    
    std::cout << "ConnectGame: Initialized successfully" << std::endl;
}

void ConnectGame::onUpdate(double dt) {
    // Game logic updates here if needed
    // For Connect4, most logic is event-driven
}

void ConnectGame::onRender(RenderSnapshot2D& snapshot) {
    renderBoard(snapshot);
    renderPieces(snapshot);
    renderHoverPiece(snapshot);
    renderUI(snapshot);
}

void ConnectGame::onEvent(const EventData& event) {
    if (gameOver) {
        // Only allow reset when game is over
        if (event.type == EventType::KeyDown && event.keyboard.key == KeyCode::R) {
            resetGame();
        }
        return;
    }
    
    handleInput(event);
}

void ConnectGame::onCleanup() {
    std::cout << "ConnectGame: Cleaning up..." << std::endl;
}

void ConnectGame::setupBoard() {
    model.reset();
    gameOver = false;
    winner = ConnectModel::Player::None;
    hoveredColumn = -1;
}

void ConnectGame::handleInput(const EventData& event) {
    switch (event.type) {
        case EventType::MouseMove: {
            // Calculate which column mouse is over
            glm::vec2 boardPos = getBoardPosition();
            glm::vec2 cellSize = getCellSize();
            
            float mouseX = static_cast<float>(event.mouse.x);
            float relativeX = mouseX - boardPos.x;
            
            if (relativeX >= 0 && relativeX < ConnectModel::COLS * cellSize.x) {
                hoveredColumn = static_cast<int>(relativeX / cellSize.x);
                hoveredColumn = glm::clamp(hoveredColumn, 0, ConnectModel::COLS - 1);
            } else {
                hoveredColumn = -1;
            }
            break;
        }
        
        case EventType::MouseButtonDown: {
            if (event.mouse.button == MouseButton::Left && hoveredColumn >= 0) {
                makeMove(hoveredColumn);
            }
            break;
        }
        
        case EventType::KeyDown: {
            // Number keys 1-7 for direct column input
            if (event.keyboard.key >= KeyCode::Num1 && event.keyboard.key <= KeyCode::Num7) {
                int column = static_cast<int>(event.keyboard.key) - static_cast<int>(KeyCode::Num1);
                if (column < ConnectModel::COLS) {
                    makeMove(column);
                }
            } else if (event.keyboard.key == KeyCode::R) {
                resetGame();
            }
            break;
        }
        
        default:
            break;
    }
}

void ConnectGame::renderBoard(RenderSnapshot2D& snapshot) {
    glm::vec2 boardPos = getBoardPosition();
    glm::vec2 cellSize = getCellSize();
    glm::vec2 boardSize = {ConnectModel::COLS * cellSize.x, ConnectModel::ROWS * cellSize.y};
    
    // Create shape renderer for this frame
    ShapeRenderer shapeRenderer(engine, whitePixelTexture);
    
    // Render board background
    glm::vec4 boardRect = {boardPos.x - 10, boardPos.y - 10, boardSize.x + 20, boardSize.y + 20};
    shapeRenderer.addRect(snapshot, boardRect, BOARD_COLOR);
    
    // Render grid cells
    for (int row = 0; row < ConnectModel::ROWS; ++row) {
        for (int col = 0; col < ConnectModel::COLS; ++col) {
            glm::vec2 cellPos = getCellPosition(row, col);
            glm::vec4 cellRect = {cellPos.x, cellPos.y, cellSize.x - 2, cellSize.y - 2};
            
            // Highlight hovered column
            glm::vec4 cellColor = EMPTY_CELL_COLOR;
            if (col == hoveredColumn && !gameOver) {
                cellColor = HOVER_COLOR;
            }
            
            shapeRenderer.addRect(snapshot, cellRect, cellColor);
        }
    }
}

void ConnectGame::renderPieces(RenderSnapshot2D& snapshot) {
    ShapeRenderer shapeRenderer(engine, whitePixelTexture);
    
    for (int row = 0; row < ConnectModel::ROWS; ++row) {
        for (int col = 0; col < ConnectModel::COLS; ++col) {
            ConnectModel::Player player = model.getCell(row, col);
            if (player != ConnectModel::Player::None) {
                glm::vec2 cellPos = getCellPosition(row, col);
                glm::vec2 cellSize = getCellSize();
                glm::vec2 pieceCenter = cellPos + cellSize * 0.5f;
                
                Circle piece = {pieceCenter, PIECE_RADIUS};
                glm::vec4 pieceColor = getPlayerColor(player);
                
                shapeRenderer.addCircle(snapshot, piece, pieceColor);
            }
        }
    }
}

void ConnectGame::renderHoverPiece(RenderSnapshot2D& snapshot) {
    if (hoveredColumn < 0 || gameOver) return;
    
    // Find the top empty row in the hovered column
    int targetRow = -1;
    for (int row = ConnectModel::ROWS - 1; row >= 0; --row) {
        if (model.getCell(row, hoveredColumn) == ConnectModel::Player::None) {
            targetRow = row;
            break;
        }
    }
    
    if (targetRow >= 0) {
        ShapeRenderer shapeRenderer(engine, whitePixelTexture);
        
        glm::vec2 cellPos = getCellPosition(targetRow, hoveredColumn);
        glm::vec2 cellSize = getCellSize();
        glm::vec2 pieceCenter = cellPos + cellSize * 0.5f;
        
        Circle hoverPiece = {pieceCenter, PIECE_RADIUS};
        glm::vec4 playerColor = getPlayerColor(model.getCurrentPlayer());
        playerColor.a = 0.5f; // Semi-transparent
        
        shapeRenderer.addCircle(snapshot, hoverPiece, playerColor);
    }
}

void ConnectGame::renderUI(RenderSnapshot2D& snapshot) {
    ShapeRenderer shapeRenderer(engine, whitePixelTexture);
    
    // Render game status
    glm::vec2 boardPos = getBoardPosition();
    glm::vec4 statusRect = {boardPos.x, boardPos.y - 40, 300, 30};
    
    if (gameOver) {
        glm::vec4 gameOverColor = {0.8f, 0.2f, 0.2f, 0.8f};
        if (winner != ConnectModel::Player::None) {
            // Winner gets their color
            gameOverColor = getPlayerColor(winner);
            gameOverColor.a = 0.8f;
        } else {
            // Draw - neutral color
            gameOverColor = {0.5f, 0.5f, 0.5f, 0.8f};
        }
        shapeRenderer.addRect(snapshot, statusRect, gameOverColor);
    } else {
        // Current player indicator
        glm::vec4 currentPlayerColor = getPlayerColor(model.getCurrentPlayer());
        currentPlayerColor.a = 0.6f;
        shapeRenderer.addRect(snapshot, statusRect, currentPlayerColor);
    }
    
    // Instructions at bottom
    glm::vec2 boardSize = {ConnectModel::COLS * getCellSize().x, ConnectModel::ROWS * getCellSize().y};
    glm::vec4 instructionsRect = {boardPos.x, boardPos.y + boardSize.y + 20, boardSize.x, 20};
    glm::vec4 instructionsColor = {0.3f, 0.3f, 0.3f, 0.7f};
    shapeRenderer.addRect(snapshot, instructionsRect, instructionsColor);
}

void ConnectGame::makeMove(int column) {
    if (gameOver || column < 0 || column >= ConnectModel::COLS) {
        return;
    }
    
    if (model.makeMove(column)) {
        checkWinCondition();
        
        if (!gameOver) {
            std::cout << "Player " << (model.getCurrentPlayer() == ConnectModel::Player::Player1 ? "1" : "2") 
                      << "'s turn" << std::endl;
        }
    }
}

void ConnectGame::resetGame() {
    setupBoard();
    std::cout << "Game reset! Player 1's turn" << std::endl;
}

void ConnectGame::checkWinCondition() {
    winner = model.checkWinner();
    if (winner != ConnectModel::Player::None) {
        gameOver = true;
        std::cout << "Player " << (winner == ConnectModel::Player::Player1 ? "1" : "2") 
                  << " wins!" << std::endl;
    } else if (model.isFull()) {
        gameOver = true;
        winner = ConnectModel::Player::None;
        std::cout << "It's a draw!" << std::endl;
    }
}

// Helper methods
glm::vec2 ConnectGame::getBoardPosition() const {
    return {BOARD_MARGIN, BOARD_MARGIN + 50}; // Extra space for UI
}

glm::vec2 ConnectGame::getCellSize() const {
    return {CELL_SIZE, CELL_SIZE};
}

glm::vec2 ConnectGame::getCellPosition(int row, int col) const {
    glm::vec2 boardPos = getBoardPosition();
    glm::vec2 cellSize = getCellSize();
    return {
        boardPos.x + col * cellSize.x + 1,
        boardPos.y + row * cellSize.y + 1
    };
}

glm::vec4 ConnectGame::getPlayerColor(ConnectModel::Player player) const {
    switch (player) {
        case ConnectModel::Player::Player1: return PLAYER1_COLOR;
        case ConnectModel::Player::Player2: return PLAYER2_COLOR;
        default: return {1.0f, 1.0f, 1.0f, 1.0f};
    }
}