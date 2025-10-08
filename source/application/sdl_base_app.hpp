
#pragma once
#include <SDL3/SDL_main.h>
#include <SDL3/SDL.h>
#include "Engine.hpp"
#include "SDLTranslate.hpp"
#include <span>
#include "BlinkMenu.hpp"
#include "GameClock.hpp"

struct EngineAppBase {
    Engine eng; SdlTranslator xlat; GameCallbacks cb;
    GameClock clock{1.0f/60.0f};
    bool inMenu = false;


    virtual bool onInit() = 0;
    virtual void onShutdown(int) {}

    bool init(){
        return onInit();
    }
    void onEvent(const SDL_Event& e){ xlat.on_event(e); }

    void iterate()
    {
        const auto& acts = xlat.flush();
        clock.update();
        eng.iterate(clock.getDeltaTime(), std::span<const EventData>(acts.data(), acts.size()));
        xlat.reset();
    }
    void shutdown(int code){ eng.quit(); onShutdown(code); }
};