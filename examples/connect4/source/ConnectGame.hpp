#pragma once

#include "BlinkGame.hpp"
#include "ConnectGui.hpp"
#include "ConnectModel.hpp"
#include "LayeredRenderer.hpp"

class ConnectGame : public BlinkGame
{
public:
    ConnectGame() = default;
    ~ConnectGame() = default;
    void init() override;
    void run() override;

    void handleEvent(const SDL_Event& event) override;
    void clicked(const Coordinate& pos);
    void hover(int, int);
    void render() override;
    void clear() override;
    void applyResolution(int, int) override;

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

    LayeredRenderer gameRenderer;
    LayeredRenderer frameRenderer;
    BlinkContext main;
    BlinkContext contextHack;

    RenderInfo background;
    RenderInfo foreground;
    RenderInfo top;
    RenderInfo bot;
    RenderInfo side1;
    RenderInfo side2;
    int res = 32;
};