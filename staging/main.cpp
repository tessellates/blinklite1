#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL_main.h>
#include "sdl_base_app.hpp"
#include "sdl_engine_entry.hpp"
#include "snake_model.hpp"
#include <BlinkMenu.hpp>

inline void submit_quads(SDL_Renderer* R, const RenderSnapshot2D& s){
    for (auto& q: s.quads){
        // decode our tiny rect encoding: position = mvp[3].xy, size = diag
        float x = q.mvp[3][0], y = q.mvp[3][1];
        float w = q.mvp[0][0], h = q.mvp[1][1];
        SDL_FRect r{ x, y, w, h };
        SDL_SetRenderDrawColor(R,
            Uint8(q.color.r*255), Uint8(q.color.g*255),
            Uint8(q.color.b*255), Uint8(q.color.a*255));
        SDL_RenderFillRect(R, &r);
    }
}

struct MyApp : EngineAppBase {
    bool onInit() override {
        eng.init(EngineConfig{800,600,"Snake Demo", "0.1","snake_demo", true});
        mainMenu.applyResolution(800,600);
        cb.step = [&](float dt, std::span<const EventData> in, RenderSnapshot2D& out){
            game.step(dt, in, out);};
        cb.render = [&](const RenderSnapshot2D& s){
            submit_quads((SDL_Renderer*)eng.getRenderer(), s);
        };
        menuCb.step = [&](float dt, std::span<const EventData> in, RenderSnapshot2D& out){
        };
        menuCb.render = [&](const RenderSnapshot2D& s){
            submit_quads((SDL_Renderer*)eng.getRenderer(), s);
            mainMenu.run((SDL_Renderer*)eng.getRenderer());
        };
        eng.setCallbacks(cb);
        return true;
    }

    
    SnakeGame game;
    BlinkMenu mainMenu; 
    GameCallbacks menuCb;
};

ENGINE_DEFINE_SDL_APP(MyApp)