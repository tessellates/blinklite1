#include <SDL.h>
#include <SDL_ttf.h>
#include <string>

class FrameRateCounter 
{
public:
    FrameRateCounter() = default;
    FrameRateCounter(SDL_Renderer* renderer, TTF_Font* font); 
    void update(); 
    void render(); 

private:
    SDL_Renderer* renderer;
    TTF_Font* font;
    SDL_Texture* textTexture = nullptr;
    SDL_Color textColor = { 255, 255, 255 };
    Uint32 prevTime = SDL_GetTicks();
    Uint32 frames = 0;
    float fps = 0;
};
