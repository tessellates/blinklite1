
#include "SpaceGame.hpp"
#include "BLApplication.hpp"
#include "TextureUtility.hpp"
#include "MovementComponent.hpp"
#include "RandomTexture.hpp"
#include "RepeatTexture.hpp"

void SpaceGame::init()
{
    renderer = BLApplication::activeRenderer();
    auto display = BLApplication::currentDisplay();
    resolutions = {{360,360}, {720, 720}, {960,960}, {1080,1080}};
    spaceBase = CreateTextureFromFile(renderer, "assets/atlas1.png");
    spaceFull = createRandomClippedTexture(renderer, spaceBase, {16,16}, {tsize,tsize});
    SDL_DestroyTexture(spaceBase);
    spaceBase = CreateTextureFromFile(renderer, "assets/atlas2.png");
    /**
    dest.h = display.h;
    = display.w/2;
    dest.y = 0;//display.h/2-dest.h/2;
    dest.x = display.w/2-dest.w/2;
    //clip = {6400-display.w,6400-display.h, display.w, display.h};
    */
    clip = {0,0,0,0};
    
    lrender1 = LayeredRenderer();
    lrender1.context.setFrameLayout({0.5, 0.5, 1, (float)display.w/display.h}, display.w, display.h);
    
    lrender1.renderer = renderer;
    lrender1.textureManager.addTexture(spaceFull);
    spaceFull = createRandomClippedTexture(renderer, spaceBase, {16,16}, {tsize,tsize});
    lrender1.textureManager.addTexture(spaceFull);
    internalUnits = {display.w, display.h};
    lrender1.context.setInternalUnits(internalUnits);
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
                clip.x -= (currentX - startX)/lrender1.context.xScale;
                clip.y -= (currentY - startY)/lrender1.context.yScale;
                startY = currentY;
                startX = currentX;
            }
            break;
        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_a) {
                SDL_DestroyTexture(spaceBase);
                SDL_DestroyTexture(spaceFull);
                spaceBase = CreateTextureFromFile(renderer, "assets/spacetileset2.png");
                spaceFull = createRandomClippedTexture(renderer, spaceBase, {16,16}, {tsize,tsize});
            }
            if (event.key.keysym.sym == SDLK_z) {
                SDL_DestroyTexture(spaceBase);
                SDL_DestroyTexture(spaceFull);
                spaceBase = CreateTextureFromFile(renderer, "assets/spacetileset.png");
                spaceFull = createRandomClippedTexture(renderer, spaceBase, {16,16}, {tsize,tsize});
            }
            if (event.key.keysym.sym == SDLK_e) {
                SDL_DestroyTexture(spaceBase);
                SDL_DestroyTexture(spaceFull);
                spaceBase = CreateTextureFromFile(renderer, "assets/spacetile3.png");
                spaceFull = createRandomClippedTexture(renderer, spaceBase, {16,16}, {tsize,tsize});
            }
            if (event.key.keysym.sym == SDLK_r) {
                SDL_DestroyTexture(spaceBase);
                SDL_DestroyTexture(spaceFull);
                spaceBase = CreateTextureFromFile(renderer, "assets/spacetile4.png");
                spaceFull = createRandomClippedTexture(renderer, spaceBase, {16,16}, {tsize,tsize});
            }
            if (event.key.keysym.sym == SDLK_LEFT) {
                scale -= 0.1;
                lrender1.context.setInternalUnits({(int)(internalUnits.x*scale),(int)(internalUnits.y*scale)});

            }
            if (event.key.keysym.sym == SDLK_RIGHT) {
                scale += 0.1;
                lrender1.context.setInternalUnits({(int)(internalUnits.x*scale),(int)(internalUnits.y*scale)});

            }
            break;
    }
}

void SpaceGame::run()
{
    
}

   
void SpaceGame::render() 
{
    auto infos = repeatTexture(0,  {-clip.x, -clip.y}, {tsize,tsize}, {0,0, (float)internalUnits.x*scale,(float)internalUnits.y*scale}, 0 );
    for (auto& info : infos)
    {
        lrender1.addRenderTarget(info);
    }
    lrender1.render();
    lrender1.clear();
    //repeatTexture(renderer, spaceFull, {-clip.x, -clip.y}, dest, scale);
    //SDL_RenderCopy(renderer, spaceFull, &clip, &dest);
}

void SpaceGame::clear()
{

}

void SpaceGame::applyResolution(int x, int y)
{
    lrender1.context.applyResolution(x, y);
    internalUnits = {lrender1.context.absoluteLayout.w, lrender1.context.absoluteLayout.h};
    lrender1.context.setInternalUnits({(int)(internalUnits.x*scale),(int)(internalUnits.y*scale)});
}
