
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
#include "BaseModule.hpp"
#include "EventStack.hpp"

struct EngineAppBase {
    SdlTranslator xlat; GameCallbacks cb;
    GameClock clock{1.0f/120.0f};

    EventData event;

    std::vector<BaseModule*> upd, draw;


    virtual bool onInit() = 0;
    virtual void onShutdown(int) {}
    virtual void onIterate(const std::span<const EventData>&) {}

    bool init(){
        onInit();
        postInit();
        return true;
    }

    void addModule(BaseModule* m)
    {
        m->initialize();
        upd.push_back(m);
        draw.push_back(m);
    }

    void postInit()
    {
        std::sort(upd.begin(),  upd.end(),  [](auto a, auto b){ return a->updateOrder < b->updateOrder; });
        std::sort(draw.begin(), draw.end(), [](auto a, auto b){ return a->drawOrder   < b->drawOrder;   });
        generateCallbacks();
    }

    void generateCallbacks()
    {
        cb.onEvent = [&](const EventData& e){
            for (auto m: upd) if (m->eventEnabled) m->onEvent(e);
        };
        cb.tick = [&](float dt){
            for (auto m: upd) if (m->iterateEnabled) m->tick(dt);
        };
        cb.extract = [&]( RenderSnapshots2D& s)
        {
            for (auto m: draw) if (m->renderEnabled) m->extract(s);
        };
        Engine::instance()->setCallbacks(cb);
    }

    void onEvent(const SDL_Event& e){ 
        ImGui_ImplSDL3_ProcessEvent(&e); 
        xlat.on_event(e, *EventStack::instance()); }

    void iterate()
    {
        while(EventStack::instance()->poll(event)) {
            Engine::instance()->onEvent(event);
        }
        //onIterate(acts);
        clock.update();
        Engine::instance()->iterate(clock.getDeltaTime());
        EventStack::instance()->reset();
    }
    void shutdown(int code){ Engine::instance()->quit(); onShutdown(code); }
};