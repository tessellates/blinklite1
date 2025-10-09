
#pragma once
#include <SDL3/SDL_main.h>
#include <SDL3/SDL.h>
#include "Engine.hpp"
#include "SDLTranslate.hpp"
#include <span>
#include "BlinkMenu.hpp"
#include "GameClock.hpp"
#include "EventStack.hpp"
#include "imgui_impl_sdl3.h"

struct EngineAppBase {
    Engine eng; SdlTranslator xlat; GameCallbacks cb; EventStack eventStack;
    GameClock clock{1.0f/120.0f};
    bool inMenu = false;


    virtual bool onInit() = 0;
    virtual void onShutdown(int) {}
    virtual void onIterate(const std::span<const EventData>&) {}

    bool init(){
        return onInit();
    }
    void onEvent(const SDL_Event& e){ 
        ImGui_ImplSDL3_ProcessEvent(&e); 
        xlat.on_event(e, eventStack); }

    void iterate()
    {
        const auto& acts = eventStack.flush();
        onIterate(acts);
        clock.update();
        eng.iterate(clock.getDeltaTime(), std::span<const EventData>(acts.data(), acts.size()));
        eventStack.reset();
    }
    void shutdown(int code){ eng.quit(); onShutdown(code); }
};