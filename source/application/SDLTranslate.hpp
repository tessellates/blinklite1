#pragma once
#include "Event.hpp"
#include <SDL3/SDL.h>

#include <vector>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>

struct SdlTranslator {
    // frame output
    std::vector<EventData> out;

    // simple bump arena for payloads (resets each frame)
    std::vector<std::byte> arena;

    // held state → digital-to-analog axes
    bool holdL=false, holdR=false, holdU=false, holdD=false;

    // helpers
    template<class T>
    const void* push(const T& v){
        // align
        const size_t align = alignof(T);
        size_t off = (arena.size() + (align-1)) & ~(align-1);
        arena.resize(off);
        // store
        const size_t start = arena.size();
        arena.resize(start + sizeof(T));
        std::memcpy(arena.data() + start, &v, sizeof(T));
        return reinterpret_cast<const void*>(arena.data() + start);
    }

    void emit_axes(){
        const float x = (holdR?1.f:0.f) - (holdL?1.f:0.f);
        const float y = (holdD?1.f:0.f) - (holdU?1.f:0.f);
        out.push_back({Event::MoveX, push(F1{ x })});
        out.push_back({Event::MoveY, push(F1{ y })});
    }

    bool on_event(const SDL_Event& e){
        switch (e.type){
        case SDL_EVENT_QUIT:
            out.push_back({Event::Quit, nullptr});
            break;

        case SDL_EVENT_WINDOW_RESIZED:
            out.push_back({Event::WindowResized, push(I2{ e.window.data1, e.window.data2 })});
            break;

        case SDL_EVENT_KEY_DOWN: {
            const SDL_Scancode s = e.key.scancode;
            if (s==SDL_SCANCODE_ESCAPE) out.push_back({Event::Back,    nullptr});
            if (s==SDL_SCANCODE_RETURN) out.push_back({Event::Confirm, nullptr});
            if (s==SDL_SCANCODE_P)      out.push_back({Event::Pause,   nullptr});

            if (s==SDL_SCANCODE_A || s==SDL_SCANCODE_LEFT)  holdL = true;
            if (s==SDL_SCANCODE_D || s==SDL_SCANCODE_RIGHT) holdR = true;
            if (s==SDL_SCANCODE_W || s==SDL_SCANCODE_UP)    holdU = true;
            if (s==SDL_SCANCODE_S || s==SDL_SCANCODE_DOWN)  holdD = true;
            emit_axes();
        } break;

        case SDL_EVENT_KEY_UP: {
            const SDL_Scancode s = e.key.scancode;
            if (s==SDL_SCANCODE_A || s==SDL_SCANCODE_LEFT)  holdL = false;
            if (s==SDL_SCANCODE_D || s==SDL_SCANCODE_RIGHT) holdR = false;
            if (s==SDL_SCANCODE_W || s==SDL_SCANCODE_UP)    holdU = false;
            if (s==SDL_SCANCODE_S || s==SDL_SCANCODE_DOWN)  holdD = false;
            emit_axes();
        } break;

        case SDL_EVENT_MOUSE_MOTION:
            out.push_back({Event::MousePos, push(V2{ float(e.motion.x), float(e.motion.y) })});
            break;

        case SDL_EVENT_MOUSE_WHEEL:
            out.push_back({Event::MouseWheel, push(V2{ float(e.wheel.x), float(e.wheel.y) })});
            break;

        case SDL_EVENT_MOUSE_BUTTON_DOWN:
            out.push_back({Event::MouseDown, push(Btn{ e.button.button })});
            break;

        case SDL_EVENT_MOUSE_BUTTON_UP:
            out.push_back({Event::MouseUp, push(Btn{ e.button.button })});
            break;

        case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
            if (e.gbutton.button == SDL_GAMEPAD_BUTTON_START) out.push_back({Event::Pause, nullptr});
            if (e.gbutton.button == SDL_GAMEPAD_BUTTON_SOUTH)     out.push_back({Event::Confirm, nullptr});
            if (e.gbutton.button == SDL_GAMEPAD_BUTTON_EAST)     out.push_back({Event::Back, nullptr});
            break;

        case SDL_EVENT_GAMEPAD_AXIS_MOTION:
            if (e.gaxis.axis == SDL_GAMEPAD_AXIS_LEFTX) out.push_back({Event::MoveX, push(F1{ e.gaxis.value/32767.0f })});
            if (e.gaxis.axis == SDL_GAMEPAD_AXIS_LEFTY) out.push_back({Event::MoveY, push(F1{ e.gaxis.value/32767.0f })});
            break;

        default:
            return false; // unhandled 
            break;
        }
        return true; // handled
    }
    void finalize_frame(){
        const float x = (holdR?1.f:0.f) - (holdL?1.f:0.f);
        const float y = (holdD?1.f:0.f) - (holdU?1.f:0.f);
        out.push_back({Event::MoveX, push(F1{ x })});
        out.push_back({Event::MoveY, push(F1{ y })});
    }
    // call once per frame after draining SDL events
    const std::vector<EventData>& flush(){
        finalize_frame();
        return out; // consumer reads, then calls reset()
    }

    // clear for next frame (must be called after consumer is done with pointers)
    void reset(){
        out.clear();
        arena.clear();
        // keep holds to preserve axes across frames
    }
};