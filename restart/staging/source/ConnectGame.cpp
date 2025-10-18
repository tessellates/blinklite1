#include "ConnectGame.hpp"
#include <iostream>
#include <glm/ext/matrix_transform.hpp>


ConnectGame::ConnectGame() {}

void ConnectGame::onInit() {
    gui = std::make_unique<ConnectGui>(*Engine::instance());
    
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
        if (event.action == Event::Confirm)
        {
            resetGame();
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
    gui->renderBoard(snapshot);
    gui->render(snapshot);
    gui->renderUI(snapshot);
    snapshots.push_back(std::move(snapshot));
}

void ConnectGame::setupGrid() {
    gameGrid.xsize = CELL_SIZE;
    gameGrid.ysize = CELL_SIZE;
    glm::vec2 boardPos = getBoardPosition();
    gui->init(gameGrid);
    gui->boardOffset = boardPos

void ConnectGame::setupBoard() {
    model = ConnectModel(); // Reset model
    hoveredColumn = -1;
}


int ConnectGame::getColumnFromMouseX(float mouseX) const {
    glm::vec2 boardPos = getBoardPosition();
    float relativeX = mouseX - boardPos.x;
    
    if (relativeX < 0 || relativeX >= ConnectModel::COLS * CELL_SIZE) {
        return -1; // Out of bounds
    }
    
    int column = static_cast<int>(relativeX / CELL_SIZE);
    return glm::clamp(column, 0, ConnectModel::COLS - 1);
}

void ConnectGame::hover(int column) {
    if (column != hoveredColumn) 
    {
        connectModel.resetCycle();
        hoveredColumn = column;
        connectGui.previewMode = true;
        connectModel.preview(column);
        connectGui.previewMode = false;
    }
    return;
}

void ConnectGame::handleInput(const EventData& event) {
    switch (event.action) {
        case Event::MousePos: {
            glm::vec2 boardPos = getBoardPosition();
            float mouseX = static_cast<const V2*>(event.data)->x;
            int newHoveredColumn = getColumnFromMouseX(mouseX);
            hover(newHoveredColumn)
        }
        
        case Event::MouseDown: {
            float mouseX = static_cast<const V2*>(event.data)->x;
            int column = getColumnFromMouseX(mouseX);
            if (column >= 0) {
                makeMove(column);
            }
            break;
        }
        
        default:
            break;
    }
}

void ConnectGame::updateGameVisuals() {

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

