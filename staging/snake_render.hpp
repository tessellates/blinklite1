#pragma once
#include <SDL3/SDL_main.h>
#include "sdl_base_app.hpp"
#include "sdl_engine_entry.hpp"
#include "snake_model.hpp"
#include <BlinkMenu.hpp>
#include <FrameRateCounter.hpp>
#include <filesystem>

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
