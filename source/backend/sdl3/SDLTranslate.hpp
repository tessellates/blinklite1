#pragma once
#include "Event.hpp"
#include "EventStack.hpp"
#include <SDL3/SDL.h>

#include <vector>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>

struct SdlTranslator {
    bool on_event(const SDL_Event& e, EventStack& stack){
        switch (e.type){
        case SDL_EVENT_QUIT:
            stack.pushNow(Event::Quit);
            break;

        case SDL_EVENT_WINDOW_RESIZED:
            stack.pushNow(Event::WindowResized, I2{ e.window.data1, e.window.data2 });
            break;

        case SDL_EVENT_KEY_DOWN: {
            const SDL_Scancode s = e.key.scancode;
            if (s==SDL_SCANCODE_ESCAPE) stack.pushNow(Event::Back);
            if (s==SDL_SCANCODE_RETURN) stack.pushNow(Event::Confirm);
            if (s==SDL_SCANCODE_P)      stack.pushNow(Event::Pause);

            if (s==SDL_SCANCODE_A || s==SDL_SCANCODE_LEFT)  stack.holdL = true;
            if (s==SDL_SCANCODE_D || s==SDL_SCANCODE_RIGHT) stack.holdR = true;
            if (s==SDL_SCANCODE_W || s==SDL_SCANCODE_UP)    stack.holdU = true;
            if (s==SDL_SCANCODE_S || s==SDL_SCANCODE_DOWN)  stack.holdD = true;
            stack.emit_axes();
        } break;

        case SDL_EVENT_KEY_UP: {
            const SDL_Scancode s = e.key.scancode;
            if (s==SDL_SCANCODE_A || s==SDL_SCANCODE_LEFT)  stack.holdL = false;
            if (s==SDL_SCANCODE_D || s==SDL_SCANCODE_RIGHT) stack.holdR = false;
            if (s==SDL_SCANCODE_W || s==SDL_SCANCODE_UP)    stack.holdU = false;
            if (s==SDL_SCANCODE_S || s==SDL_SCANCODE_DOWN)  stack.holdD = false;
            stack.emit_axes();
        } break;

        // ----- Mouse -----
        case SDL_EVENT_MOUSE_MOTION:
            stack.pushNow(Event::MousePos, V2{ float(e.motion.x), float(e.motion.y) });
            break;

        case SDL_EVENT_MOUSE_WHEEL:
            stack.pushNow(Event::MouseWheel, V2{ float(e.wheel.x), float(e.wheel.y) });
            break;

        case SDL_EVENT_MOUSE_BUTTON_DOWN:
            stack.pushNow(Event::MouseDown, Btn{
                static_cast<MouseButton>(e.button.button),
                e.button.x,
                e.button.y
            });
            break;

        case SDL_EVENT_MOUSE_BUTTON_UP:
            stack.pushNow(Event::MouseUp, Btn{
                static_cast<MouseButton>(e.button.button),
                e.button.x,
                e.button.y
            });
            break;


        // ----- Gamepad -----
        case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
            if (e.gbutton.button == SDL_GAMEPAD_BUTTON_START)
                stack.pushNow(Event::Pause);
            if (e.gbutton.button == SDL_GAMEPAD_BUTTON_SOUTH)
                stack.pushNow(Event::Confirm);
            if (e.gbutton.button == SDL_GAMEPAD_BUTTON_EAST)
                stack.pushNow(Event::Back);
            break;

        case SDL_EVENT_GAMEPAD_AXIS_MOTION:
            if (e.gaxis.axis == SDL_GAMEPAD_AXIS_LEFTX)
                stack.pushNow(Event::MoveX, F1{ e.gaxis.value / 32767.0f });
            if (e.gaxis.axis == SDL_GAMEPAD_AXIS_LEFTY)
                stack.pushNow(Event::MoveY, F1{ e.gaxis.value / 32767.0f });
            break;

        default:
            return false; // unhandled
        }

        return true; // handled
    }
};