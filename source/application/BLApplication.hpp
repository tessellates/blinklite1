#pragma once

#include "Singleton.hpp"
#include <SDL.h>
#include "imgui.h"

class BlinkGame;

class BLApplication : public Singleton<BLApplication>
{
   friend class Singleton<BLApplication>; // Grant base access to constructor.
public:
    // boiler-plate singleton
    BLApplication(BLApplication &other) = delete;
    void operator=(const BLApplication &) = delete;

    int run();
    void init();
    void init(BlinkGame* blinkGame);

private:
    BLApplication();
    ~BLApplication();

    void loop();

    SDL_Event event;
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    BlinkGame* game;
    //BLSDLRenderer blsdlRenderer;
    ImGuiIO* io_;

    bool imguiToggle = false;
    bool isRunning = true;
    bool isBloom = false;

    SDL_Texture* test;
    SDL_Texture* testmod;
    SDL_Surface* surfaceMod;
};
