#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL_main.h>
#include "sdl_base_app.hpp"
#include "sdl_engine_entry.hpp"
#include "snake_model.hpp"
#include "snake_render.hpp"
#include <BlinkMenu.hpp>
#include <FrameRateCounter.hpp>
#include <filesystem>

struct MyApp : EngineAppBase {
    bool onInit() override {
        eng.init(EngineConfig{800,600,"Snake Demo", "0.1","snake_demo", true});
        mainMenu.applyResolution(800,600);
        mainMenu.init(&eventStack, &eng);
        cb.step = [&](float dt, std::span<const EventData> in, RenderSnapshot2D& out){
            game.step(dt, in, out);};
        cb.render = [&](const RenderSnapshot2D& s)
        {
            submit_quads((SDL_Renderer*)eng.getRenderer(), s);
        };
        menuCb.step = [&](float dt, std::span<const EventData> in, RenderSnapshot2D& out)
        {
        };
        menuCb.render = [&](const RenderSnapshot2D& s){
            submit_quads((SDL_Renderer*)eng.getRenderer(), s);
            mainMenu.run((SDL_Renderer*)eng.getRenderer());
        };
        eng.setCallbacks(cb);
        TTF_Font* font = TTF_OpenFont("assets/Arial.ttf", 12);
        if (!font) {
            std::cout << SDL_GetError() << std::endl;
        }
        frc = FrameRateCounter((SDL_Renderer*)eng.getRenderer(), font);
        return true;
    }

    void onIterate(const std::span<const EventData>& acts) override {
        frc.update();

        for (auto& a: acts){
            if (a.action == Event::Back){
                inMenu = !inMenu;
                eng.setCallbacks(inMenu? menuCb : cb);
            }
            if (a.action == Event::FrameRateToggle){
                if (mainMenu.frameRate) 
                {
                    menuCb.render = [&](const RenderSnapshot2D& s){
                        submit_quads((SDL_Renderer*)eng.getRenderer(), s);
                        mainMenu.run((SDL_Renderer*)eng.getRenderer());
                        frc.render();
                    };
                    cb.render = [&](const RenderSnapshot2D& s){
                        submit_quads((SDL_Renderer*)eng.getRenderer(), s);
                        frc.render();
                    };
                }
                else
                {
                    menuCb.render = [&](const RenderSnapshot2D& s){
                        submit_quads((SDL_Renderer*)eng.getRenderer(), s);
                        mainMenu.run((SDL_Renderer*)eng.getRenderer());
                    };
                    cb.render = [&](const RenderSnapshot2D& s){
                        submit_quads((SDL_Renderer*)eng.getRenderer(), s);
                    };
                }
                eng.setCallbacks(inMenu? menuCb : cb);
            }
        }
    }

    bool inMenu = false;
    SnakeGame game;
    BlinkMenu mainMenu; 
    GameCallbacks menuCb;
    FrameRateCounter frc;
};

ENGINE_DEFINE_SDL_APP(MyApp)