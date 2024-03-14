#pragma once

#include "Singleton.hpp"
#include "BLSDLRenderer.hpp"
#include "SDLUserEvents.hpp"
#include "GameClock.hpp"
#include "FrameRateCounter.hpp"

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
    void updateDisplay();

    static BlinkGame* activeGame();
    static SDL_Renderer* activeRenderer();
    static const SDL_Rect& currentDisplay();
    static void changeWindow(const std::pair<int,int>& );
    void changeWindow();

    static bool isFullscreen;
    static int currentResolution;
    static float deltaTime;
    static bool frameRate;
    void toggleFullscreen();
    std::vector<std::pair<int,int>> resolutions;

private:
    BLApplication();
    ~BLApplication();

    void loop();
    void correctDisplay();

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

    int xResolution;
    int yResolution;

    SDL_Rect display;

    float scaleFactor;

    GameClock clock;

    FrameRateCounter frc;
    
    bool hasToggled = true;
};
