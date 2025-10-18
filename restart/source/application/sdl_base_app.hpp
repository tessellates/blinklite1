
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
        cb.tick = [&](float dt, const std::span<const EventData>& in){
            for (auto m: upd) if (m->iterateEnabled) m->tick(dt, in);
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
        const auto& acts = EventStack::instance()->flush();
        onIterate(acts);
        clock.update();
        Engine::instance()->iterate(clock.getDeltaTime(), std::span<const EventData>(acts.data(), acts.size()));
        EventStack::instance()->reset();
    }
    void shutdown(int code){ Engine::instance()->quit(); onShutdown(code); }
};