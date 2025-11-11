#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL_main.h>
#include "sdl_base_app.hpp"
#include "sdl_engine_entry.hpp"
#include "battlezone_model.hpp"
#include <BlinkMenu.hpp>
#include <FrameRateCounter.hpp>

struct BattleZoneGameModule : public BaseModule
{
    BattleZoneModel game;
    BattleZoneContext context{1280.0f, 720.0f, 360.0f};
    RenderSnapshot2D rs;
    bool triggerExtract = false;
    
    void extract(RenderSnapshots2D& s) override
    {
        rs.context.viewport = {0, 0, 1280, 720};
        battlezone_extract(game, context, rs);
        s.push_back(rs);
        triggerExtract = false;
    }

    void tick(float dt) override
    {
        battlezone_tick(game, dt);
    }

    void onEvent(const EventData& event) override
    {
        battlezone_input(game, event);
    }
};

struct MyApp : EngineAppBase {
    BattleZoneGameModule game;
    
    bool onInit() override {
        Engine::instance()->init(EngineConfig{1280, 720, "BattleZone", "0.1", "battlezone_demo", true});
        game.drawOrder = 0;
        game.updateOrder = 0;
        addModule(&game);
        return true;
    }

    void onIterate(float dt) override {
        // Frame logic if needed
    }

    void onEvent(const EventData& a) override {
        // Handle app-level events
    }
};

ENGINE_DEFINE_SDL_APP(MyApp)