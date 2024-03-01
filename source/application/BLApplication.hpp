#pragma once

#include "Singleton.hpp"
#include "BLSDLRenderer.hpp"
#include <SDL.h>
#include "imgui.h"

class BlinkGame;
class BlinkMenu;

class BLApplication : public Singleton<BLApplication>
{
   friend class Singleton<BLApplication>; // Grant base access to constructor.
public:
    // boiler-plate singleton
    BLApplication(BLApplication &other) = delete;
    void operator=(const BLApplication &) = delete;

    int run();
    void init(bool test = true);
    void init(BlinkGame* blinkGame);

    static BlinkGame* activeGame();
    static BLSDLRenderer& mainRenderer();

private:
    BLApplication();
    ~BLApplication();

    void loop();

    SDL_Event event;
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    BlinkGame* game = nullptr;
    BlinkMenu* menu = nullptr;

    ImGuiIO* io_;

    bool imguiToggle = false;
    bool isRunning = true;
    bool isInit = false;

    SDL_Texture* test;
    BLSDLRenderer blRenderer;

    int x_resolution;
    int y_resolution;

    float scaleFactor = 1;

    int x_window;
    int y_window;

    SDL_Rect display;
};
