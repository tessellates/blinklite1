#include "ConnectGame.hpp"
#include <iostream>

ConnectGame::ConnectGame() {}

void ConnectGame::onInit() {
    gui = std::make_unique<ConnectGui>(*engine);
    
    // Connect model callbacks to GUI
    model.addConnectEntity.push_back([this](const Coordinate& pos, int color) {
        gui->addConnectEntity(pos, color);
    });
    
    model.removeConnectEntity.push_back([this](const Coordinate& pos) {
        gui->removeConnectEntity(pos);
    });
    
    model.changeConnectEntity.push_back([this](const Coordinate& pos, int color) {
        gui->changeConnectEntity(pos, color);
    });
    
    setupGrid();
    setupBoard();
}

void ConnectGame::tick(float dt, std::span<const EventData> events) {    
    // Handle events
    for (const auto& event : events) {
        if (model.gameOver) {
            if (event.type == EventType::KeyDown && event.keyboard.key == KeyCode::R) {
                resetGame();
            }
            continue;
        }
        handleInput(event);
    }
    
    // Update game
    gui->update(dt);
    updateGameVisuals();
}

void ConnectGame::extract(RenderSnapshots2D& snapshots) {    
    RenderSnapshot2D snapshot;    
    renderBoard(snapshot);
    gui->render(snapshot);
    renderUI(snapshot);
    snapshots.push_back(std::move(snapshot));
}

void ConnectGame::setupGrid() {
    gameGrid.xsize = CELL_SIZE;
    gameGrid.ysize = CELL_SIZE;
    glm::vec2 boardPos = getBoardPosition();
    gameGrid.offsetX = boardPos.x;
    gameGrid.offsetY = boardPos.y;
    gui->init(gameGrid);
}

void ConnectGame::setupBoard() {
    model = ConnectModel(); // Reset model
    hoveredColumn = -1;
}

void ConnectGame::handleInput(const EventData& event) {
    switch (event.type) {
        case EventType::MousePos: {
            glm::vec2 boardPos = getBoardPosition();
            V2 mousePos = static_cast<V2>(event.data);
            float relativeX = mousePos.x - boardPos.x;
            
            int newHoveredColumn = -1;
            if (relativeX >= 0 && relativeX < ConnectModel::COLS * CELL_SIZE) {
                newHoveredColumn = static_cast<int>(relativeX / CELL_SIZE);
                newHoveredColumn = glm::clamp(newHoveredColumn, 0, ConnectModel::COLS - 1);
            }
            
            if (newHoveredColumn != hoveredColumn) {
                hoveredColumn = newHoveredColumn;
            }
            break;
        }
        
        case EventType::MouseButtonDown: {
            if (hoveredColumn >= 0) {
                makeMove(hoveredColumn);
            }
            break;
        }
        
        default:
            break;
    }
}

void ConnectGame::renderBoard(RenderSnapshot2D& snapshot) {
    glm::vec2 boardPos = getBoardPosition();
    glm::vec2 boardSize = {ConnectModel::COLS * CELL_SIZE, ConnectModel::ROWS * CELL_SIZE};
    
    // Board background
    addRectQuad(snapshot, {boardPos.x - 10, boardPos.y - 10, boardSize.x + 20, boardSize.y + 20}, BOARD_COLOR);
    
    // Grid cells
    for (int row = 0; row < ConnectModel::ROWS; ++row) {
        for (int col = 0; col < ConnectModel::COLS; ++col) {
            glm::vec2 cellPos = {boardPos.x + col * CELL_SIZE, boardPos.y + row * CELL_SIZE};
            glm::vec4 cellRect = {cellPos.x + 1, cellPos.y + 1, CELL_SIZE - 2, CELL_SIZE - 2};
            
            glm::vec4 cellColor = EMPTY_CELL_COLOR;
            if (col == hoveredColumn && !model.gameOver) {
                cellColor = HOVER_COLOR;
            }
            
            addRectQuad(snapshot, cellRect, cellColor);
        }
    }
}

void ConnectGame::renderUI(RenderSnapshot2D& snapshot) {
    glm::vec2 boardPos = getBoardPosition();
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

void ConnectGame::addRectQuad(RenderSnapshot2D& snapshot, glm::vec4 rect, glm::vec4 color) {
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

void ConnectGame::updateGameVisuals() {
    // Clear preview pieces
    gui->setPreviewMode(true);
    for (int row = 0; row < ConnectModel::ROWS; ++row) {
        gui->removeConnectEntity(Coordinate(hoveredColumn, row));
    }
    
    // Add hover preview if valid
    if (hoveredColumn >= 0 && !model.gameOver) {
        if (model.board[hoveredColumn].size() < ConnectModel::ROWS) {
            int targetRow = ConnectModel::ROWS - static_cast<int>(model.board[hoveredColumn].size()) - 1;
            gui->addConnectEntity(Coordinate(hoveredColumn, targetRow), model.player);
        }
    }
    
    gui->setPreviewMode(false);
}

void ConnectGame::makeMove(int column) {
    if (model.gameOver || column < 0 || column >= ConnectModel::COLS) {
        return;
    }
    
    model.play(column);
    hoveredColumn = -1;
    
    if (model.gameOver) {
        std::cout << "Player " << (!model.player ? "1" : "2") << " wins!" << std::endl;
    }
}

void ConnectGame::resetGame() {
    setupBoard();
    std::cout << "Game reset!" << std::endl;
}

glm::vec2 ConnectGame::getBoardPosition() const {
    return {BOARD_MARGIN, BOARD_MARGIN + 50};
}

glm::vec4 ConnectGame::getPlayerColor(int player) const {
    switch (player) {
        case 0: return {1.0f, 0.2f, 0.2f, 1.0f}; // Red
        case 1: return {1.0f, 1.0f, 0.2f, 1.0f}; // Yellow
        default: return {1.0f, 1.0f, 1.0f, 1.0f};
    }
}