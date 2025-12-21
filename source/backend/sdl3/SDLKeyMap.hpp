#pragma once
#include "Event.hpp"
#include <SDL3/SDL.h>

class SDLKeyMapper {
private:
    static KeyboardKey keyMap[SDL_SCANCODE_COUNT];
    static bool initialized;
    
    static void initializeMap() {
        if (initialized) return;
        
        // Initialize all to invalid/unmapped
        for (int i = 0; i < SDL_SCANCODE_COUNT; i++) {
            keyMap[i] = KEY_NONE; // fallback
        }
        
        // Map the keys we care about
        keyMap[SDL_SCANCODE_A] = KEY_A;
        keyMap[SDL_SCANCODE_B] = KEY_B;
        keyMap[SDL_SCANCODE_C] = KEY_C;
        keyMap[SDL_SCANCODE_D] = KEY_D;
        keyMap[SDL_SCANCODE_E] = KEY_E;
        keyMap[SDL_SCANCODE_F] = KEY_F;
        keyMap[SDL_SCANCODE_G] = KEY_G;
        keyMap[SDL_SCANCODE_H] = KEY_H;
        keyMap[SDL_SCANCODE_I] = KEY_I;
        keyMap[SDL_SCANCODE_J] = KEY_J;
        keyMap[SDL_SCANCODE_K] = KEY_K;
        keyMap[SDL_SCANCODE_L] = KEY_L;
        keyMap[SDL_SCANCODE_M] = KEY_M;
        keyMap[SDL_SCANCODE_N] = KEY_N;
        keyMap[SDL_SCANCODE_O] = KEY_O;
        keyMap[SDL_SCANCODE_P] = KEY_P;
        keyMap[SDL_SCANCODE_Q] = KEY_Q;
        keyMap[SDL_SCANCODE_R] = KEY_R;
        keyMap[SDL_SCANCODE_S] = KEY_S;
        keyMap[SDL_SCANCODE_T] = KEY_T;
        keyMap[SDL_SCANCODE_U] = KEY_U;
        keyMap[SDL_SCANCODE_V] = KEY_V;
        keyMap[SDL_SCANCODE_W] = KEY_W;
        keyMap[SDL_SCANCODE_X] = KEY_X;
        keyMap[SDL_SCANCODE_Y] = KEY_Y;
        keyMap[SDL_SCANCODE_Z] = KEY_Z;
        
        keyMap[SDL_SCANCODE_0] = KEY_0;
        keyMap[SDL_SCANCODE_1] = KEY_1;
        keyMap[SDL_SCANCODE_2] = KEY_2;
        keyMap[SDL_SCANCODE_3] = KEY_3;
        keyMap[SDL_SCANCODE_4] = KEY_4;
        keyMap[SDL_SCANCODE_5] = KEY_5;
        keyMap[SDL_SCANCODE_6] = KEY_6;
        keyMap[SDL_SCANCODE_7] = KEY_7;
        keyMap[SDL_SCANCODE_8] = KEY_8;
        keyMap[SDL_SCANCODE_9] = KEY_9;
        
        keyMap[SDL_SCANCODE_SPACE] = KEY_SPACE;
        keyMap[SDL_SCANCODE_RETURN] = KEY_ENTER;
        keyMap[SDL_SCANCODE_ESCAPE] = KEY_ESCAPE;
        keyMap[SDL_SCANCODE_UP] = KEY_UP;
        keyMap[SDL_SCANCODE_DOWN] = KEY_DOWN;
        keyMap[SDL_SCANCODE_LEFT] = KEY_LEFT;
        keyMap[SDL_SCANCODE_RIGHT] = KEY_RIGHT;
        keyMap[SDL_SCANCODE_LSHIFT] = KEY_LSHIFT;
        keyMap[SDL_SCANCODE_RSHIFT] = KEY_RSHIFT;
        
        initialized = true;
    }
    
public:
    static KeyboardKey translateKey(SDL_Scancode sdlKey) {
        initializeMap();
        return (sdlKey < SDL_SCANCODE_COUNT) ? keyMap[sdlKey] : KEY_NONE;
    }
};

KeyboardKey SDLKeyMapper::keyMap[SDL_SCANCODE_COUNT];
bool SDLKeyMapper::initialized = false;