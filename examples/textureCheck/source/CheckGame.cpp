
#include "CheckGame.hpp"
#include "BLApplication.hpp"
#include "TextureUtility.hpp"
#include "MovementComponent.hpp"

void CheckGame::init()
{
    renderer = BLApplication::activeRenderer();
    resolutions = {{360,360}, {720, 720}, {960,960}, {1080,1080}};
    /*
    test1 = CreateTextureFromFile(renderer, "assets/test.png");
    test2 = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 720, 720);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetTextureBlendMode(test2, SDL_BLENDMODE_BLEND);
    SDL_SetRenderTarget(renderer, test2);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, test1, nullptr, nullptr);
    SDL_SetRenderTarget(renderer, NULL);
    test3 = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 720, 720);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
    SDL_SetTextureBlendMode(test3, SDL_BLENDMODE_NONE);
    SDL_SetRenderTarget(renderer, test3);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, test1, nullptr, nullptr);
    SDL_SetRenderTarget(renderer, NULL);
    */

    test1 = CreateColorTexture(renderer, SDL_MapRGBA(SDL_AllocFormat(SDL_PIXELFORMAT_RGBA8888), 255, 255, 0, 122));
    SDL_SetTextureBlendMode(test1, SDL_BLENDMODE_NONE);

    test2 = CreateColorTexture(renderer, SDL_MapRGBA(SDL_AllocFormat(SDL_PIXELFORMAT_RGBA8888), 255, 255, 0, 122));
    SDL_SetTextureBlendMode(test2, SDL_BLENDMODE_BLEND);

    test3 = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 2, 2);
    SDL_SetTextureBlendMode(test3, SDL_BLENDMODE_NONE);

    SDL_SetRenderTarget(renderer, test3);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, test1, nullptr, nullptr);

    test4 = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 2, 2);
    SDL_SetTextureBlendMode(test4, SDL_BLENDMODE_NONE);

    SDL_SetRenderTarget(renderer, test4);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, test2, nullptr, nullptr);

    SDL_SetTextureBlendMode(test3, SDL_BLENDMODE_BLEND);
    SDL_SetTextureBlendMode(test4, SDL_BLENDMODE_BLEND);

    SDL_SetRenderTarget(renderer, NULL);


}

void CheckGame::handleEvent(const SDL_Event& event)
{
    switch (event.type) 
    {
        case SDL_KEYDOWN: 
        {
            if (event.key.keysym.sym == SDLK_RIGHT)
            {
                mode++;
                mode = std::abs(mode % 4);
            }
            if (event.key.keysym.sym == SDLK_LEFT)
            {
                mode--;
                mode = std::abs(mode % 4);     
            }
            if (event.key.keysym.sym == SDLK_a)
            {
                color--;
                if (color < 0)
                {
                    color = 0;
                }
            }
            if (event.key.keysym.sym == SDLK_z)
            {
                color++;
                if (color > 255)
                {
                    color = 255;
                }  
            }
            std::cout << "MODE=" << mode << std::endl;
            break;
        }
        default:
            break;
    }
}

void CheckGame::run()
{

}

   
void CheckGame::render() 
{
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
    SDL_SetRenderDrawColor(renderer, color,color,color,0);
    SDL_RenderClear(renderer);
    switch(mode)
    {
        case 0:
        {
            SDL_RenderCopy(renderer, test1, nullptr, &dest);
            break;
        }
        case 1:
        {
            SDL_RenderCopy(renderer, test2, nullptr, &dest);
            break;
        }
        case 2:
        {
            SDL_RenderCopy(renderer, test3, nullptr, &dest);
            break;
        }
        case 3:
        {
            SDL_RenderCopy(renderer, test4, nullptr, &dest);
            break;
        }
    }
}

void CheckGame::clear()
{
}

void CheckGame::applyResolution(int x, int y)
{
    dest.h = y;
    dest.w = y;
    dest.y = y/2-dest.h/2;
    dest.x = x/2-dest.w/2;
}
