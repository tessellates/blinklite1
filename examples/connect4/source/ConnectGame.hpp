#pragma once

#include "BlinkGame.hpp"
#include "ConnectGui.hpp"
#include "ConnectModel.hpp"

class ConnectGame : public BlinkGame
{
public:
    ConnectGame() = default;
    ~ConnectGame() = default;
    void init() override;
    void run() override;

    void handleEvent(const SDL_Event& event) override;
    void clicked(const Coordinate& pos);
    void hover(const Coordinate& pos);

    void forward();
    void backward();

    ImGuiWindowFlags windowFlags;
    
    ConnectGui connectGui;
    ConnectModel connectModel;
    bool play = true;

    ImVec2 padding;
    bool isHoverActive = false; // State variable to control hover
    int currentPreview = -1;
    
    Grid grid;

    BLSDLRenderer* gameRenderer;
    RenderInfo background;
    RenderInfo foreground;
    RenderInfo top;
    RenderInfo bot;
    RenderInfo side1;
    RenderInfo side2;
};