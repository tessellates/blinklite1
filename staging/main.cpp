#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL_main.h>
#include "sdl_base_app.hpp"
#include "sdl_engine_entry.hpp"
#include "snake_model.hpp"
#include <BlinkMenu.hpp>
#include <FrameRateCounter.hpp>
#include <filesystem>


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
};

struct SnakeGameModule : public BaseModule
{
    SnakeGame game;
    RenderSnapshot2D rs;
    void extract( RenderSnapshots2D& s) override
    {
        snake_extract(game.model, game.context, rs, game.moved);
        s.push_back(rs);
    }

    void tick(float dt) override
    {
        game.tick(dt);
    }

    void onEvent(const EventData& event) override
    {
        game.onEvent(event);
    }
};


struct MyApp : EngineAppBase {

    bool inMenu = false;
    SnakeGameModule game;
    BlinkMenu mainMenu; 
    FrameRateCounterModule frc;

    bool onInit() override {
        Engine::instance()->init(EngineConfig{800,600,"Snake Demo", "0.1","snake_demo", true});
        mainMenu.applyResolution(800,600);
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

    void onIterate(const std::span<const EventData>& acts) override {
        frc.frc.update();

        for (auto& a: acts){
            if (a.action == Event::Back){
                inMenu = !inMenu;
                mainMenu.renderEnabled = inMenu;
                game.iterateEnabled = !inMenu;
            }
            if (a.action == Event::FrameRateToggle){
                frc.renderEnabled = !frc.renderEnabled;
            }
            if (a.action == Event::WindowResized)
            {
                mainMenu.applyResolution(Engine::instance()->getWindowSize().x, Engine::instance()->getWindowSize().y);
                game.game.context = SnakeContext{ {16,12,1}, (float)Engine::instance()->getWindowSize().x, (float)Engine::instance()->getWindowSize().y };
            }
        }
    }
};

ENGINE_DEFINE_SDL_APP(MyApp)