#pragma once

#include "Singleton.hpp"
#include "BLSDLRenderer.hpp"

class BlinkGame;

class BLApplication : public Singleton<BLApplication>
{
   friend class Singleton<BLApplication>; // Grant base access to constructor.
public:
    // boiler-plate singleton
    BLApplication(BLApplication &other) = delete;
    void operator=(const BLApplication &) = delete;

    void run();
    void init(BlinkGame* blinkGame);

private:
    BLApplication();
    ~BLApplication();

    SDL_Event event;
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    BLSDLRenderer blsdlRenderer;
    ImGuiIO* io_;
};
