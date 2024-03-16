
#include "SpaceGame.hpp"
#include "BLApplication.hpp"
#include "TextureUtility.hpp"
#include "MovementComponent.hpp"
#include "RandomTexture.hpp"

void SpaceGame::init()
{
    renderer = BLApplication::activeRenderer();
    auto display = BLApplication::currentDisplay();
    resolutions = {{360,360}, {720, 720}, {960,960}, {1080,1080}};
    spaceBase = CreateTextureFromFile(renderer, "assets/spacetileset.png");
    spaceFull = createRandomClippedTexture(renderer, spaceBase, {16,16}, {6400,6400});
    dest.h = display.h;
    dest.w = display.w;
    dest.y = display.h/2-dest.h/2;
    dest.x = display.w/2-dest.w/2;
    clip = {6400-display.w,6400-display.h, display.w, display.h};
}

void SpaceGame::handleEvent(const SDL_Event& event)
{
    switch(event.type)
    {
        case SDL_MOUSEBUTTONDOWN:
            if (event.button.button == SDL_BUTTON_LEFT) {
                isMouseDown = 1;
                startX = event.button.x;
                startY = event.button.y;
                currentX = startX;
                currentY = startY;
                distance = 0.0f; // Reset distance
            }
            break;
        case SDL_MOUSEBUTTONUP:
            if (event.button.button == SDL_BUTTON_LEFT) {
                isMouseDown = 0;
                //clip.x -= (currentX - startX)/2;
                //clip.y -= (currentY - startY)/2;
            }
            break;
        case SDL_MOUSEMOTION:
            if (isMouseDown) {
                currentX = event.motion.x;
                currentY = event.motion.y;
                clip.x -= (currentX - startX);
                clip.y -= (currentY - startY);
                startY = currentY;
                startX = currentX;
            }
            break;
    }
}

void SpaceGame::run()
{

}

   
void SpaceGame::render() 
{
    SDL_RenderCopy(renderer, spaceFull, &clip, &dest);
}

void SpaceGame::clear()
{
}

void SpaceGame::applyResolution(int x, int y)
{

}
