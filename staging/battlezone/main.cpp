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

struct FrameRateCounterModule : public BaseModule
{
    FrameRateCounter frc;
    void extract( RenderSnapshots2D& s) override
    {
        RenderSnapshot2D rs;
        rs.customRender = [this]()
        {
            frc.render();
        };
        s.push_back(rs);
    }
    void tick(float dt) override {
        frc.update();
    }

};


struct MyApp : EngineAppBase {
    BattleZoneGameModule game;
    bool inMenu = false;
    BlinkMenu mainMenu; 
    FrameRateCounterModule frc;
    bool onInit() override {
        Engine::instance()->init(EngineConfig{1280, 720, "BattleZone", "0.1", "battlezone_demo", true});

        mainMenu.applyResolution(1280,720);
        TTF_Font* font = TTF_OpenFont("assets/Arial.ttf", 12);
        if (!font) {
            std::cout << SDL_GetError() << std::endl;
        }
        frc.frc = FrameRateCounter((SDL_Renderer*)Engine::instance()->getRenderer(), font);

        game.drawOrder = 0;
        game.updateOrder = 0;

        mainMenu.drawOrder = 1;
        mainMenu.updateOrder = 1;
        mainMenu.renderEnabled = false;
        frc.drawOrder = 2;
        frc.updateOrder = 2;
        frc.renderEnabled = false;

        addModule(&game);
        addModule(&mainMenu);
        addModule(&frc);

        return true;
    }

    void onIterate(float dt) override {
        // Frame logic if needed
    }

    void onEvent(const EventData& a) override {
        if (a.action == Event::Back){
            inMenu = !inMenu;
            mainMenu.renderEnabled = inMenu;
            game.iterateEnabled = !inMenu;
            game.eventEnabled = !inMenu;
        }
        if (a.action == Event::FrameRateToggle){
            frc.renderEnabled = !frc.renderEnabled;
        }
    }
};

ENGINE_DEFINE_SDL_APP(MyApp)