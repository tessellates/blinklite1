#include <SDL.h>
#include <SDL_ttf.h>
#include <string>
#include "FrameRateCounter.hpp"
#include <iostream>

FrameRateCounter::FrameRateCounter(SDL_Renderer* renderer, TTF_Font* font) : renderer(renderer), font(font) {}

void FrameRateCounter::update() 
{
    Uint32 currentTime = SDL_GetTicks();
    Uint32 deltaTime = currentTime - prevTime;
    frames++;
    if (deltaTime >= 1000) {
        fps = frames / (deltaTime / 1000.0f);
        frames = 0;
        prevTime = currentTime;
    }
}

void FrameRateCounter::render() {
    // Render frame rate text
    std::string fpsText = "Frame rate: " + std::to_string(fps);
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, fpsText.c_str(), textColor);
    if (textSurface) {
        if (textTexture)
            SDL_DestroyTexture(textTexture);
        textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        SDL_Rect textRect = { 10, 10, textSurface->w, textSurface->h };
        SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);
        SDL_FreeSurface(textSurface);
    }
}
