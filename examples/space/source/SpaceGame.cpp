
#include "SpaceGame.hpp"
#include "BLApplication.hpp"
#include "TextureUtility.hpp"
#include "MovementComponent.hpp"
#include "RandomTexture.hpp"
#include "RepeatTexture.hpp"
#include "RenderInfo.hpp"

void SpaceGame::init()
{
    renderer = BLApplication::activeRenderer();
    auto display = BLApplication::currentDisplay();
    resolutions = {{400,600}, {600,900}};
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
    dest.h = display.h;
    dest.w = display.h/2;
    dest.y = 0;//display.h/2-dest.h/2;
    dest.x = display.w/2-dest.w/2;
    clip = {0,0,0,0};
    
    lrender1 = LayeredRenderer();
    lrender1.context.setFrameLayout({0.5, 0.5, 1, (float)display.w/display.h}, display.w, display.h);
    lrender1.context.setFrameLayout({0.5, 0.5, 1, 0.5}, display.w, display.h);
    
    lrender1.renderer = renderer;
    lrender1.textureManager.addTexture(spaceFull);
    spaceFull = createRandomClippedTexture(renderer, spaceBase, {16,16}, {tsize,tsize});
    lrender1.textureManager.addTexture(spaceFull);
    internalUnits = {display.w, display.h};
    lrender1.context.setInternalUnits(internalUnits);

    mid.setFrameLayout({0.5, 0.5, 1, 0.5}, display.w, display.h);
    midline.setFrameLayout({0.5, 0.5, 1, 1/(float)display.h}, display.w, display.h);

    entityRenderer1 = LayeredRenderer();
    entityRenderer1.renderer = renderer;
    entityRenderer1.context.setFrameLayout({0, 0, 1, 0.5, NEGATIVE, NEGATIVE},  display.h/2, display.h);
    entityRenderer1.target = SDL_CreateTexture(entityRenderer1.renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, display.h/2, display.h)<;
    entityRenderer1.backgroundColor = {0,0,0,0};
    SDL_SetTextureBlendMode(entityRenderer1.target, SDL_BLENDMODE_BLEND);
    entityRenderer1.context.setInternalUnits({ display.h/4, display.h/2});
    entityRenderer1.textureManager.addTexture(CreateTextureFromFile(renderer, "assets/basevehicle-4.png"));

    playerNode.absolute = {{(float)display.h/8, (float)display.h/2-64}};
    playerTexture.dest = {-11, -8.5, 22, 17};
    //playerTexture.dest = {0, 0, 22, 17};
    playerTexture.layerID = 1;
    playerTexture.textureID = 0;
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
                scale -= 0.1;
                lrender1.context.setInternalUnits({(int)(internalUnits.x*scale),(int)(internalUnits.y*scale)});

            }
            if (event.key.keysym.sym == SDLK_z) {
                scale += 0.1;
                lrender1.context.setInternalUnits({(int)(internalUnits.x*scale),(int)(internalUnits.y*scale)});

            }

            switch (event.key.keysym.sym) 
            {
                case SDLK_LSHIFT:
                    speed *= shiftmod;
                    accel.y *= shiftmod;
                    accel.x *= shiftmod;
                    break;
                case SDLK_UP:
                    accel.y = -speed;
                    break;
                case SDLK_DOWN:
                    accel.y = speed;
                    break;
                case SDLK_LEFT:
                    accel.x = -speed;
                    break;
                case SDLK_RIGHT:
                    accel.x = speed;
                    break;
                default:
                    break;
            }
            break;
        case SDL_KEYUP:
            switch (event.key.keysym.sym) 
            {
                case SDLK_UP:
                case SDLK_DOWN:
                    accel.y = 0;
                    break;
                case SDLK_LEFT:
                case SDLK_RIGHT:
                    accel.x = 0;
                    break;
                case SDLK_LSHIFT:
                    speed /= shiftmod;
                    accel.x /= shiftmod;
                    accel.y /= shiftmod;
                    break;
                default:
                    break;
            }
            break;
    }
}

void SpaceGame::run()
{
    velocity += accel;
    if (playerNode.absolute.position.x + velocity.x < 20 || 225-20 < playerNode.absolute.position.x + velocity.x)
    {
        velocity.x = 0;
    }
    if (playerNode.absolute.position.y + velocity.y < 20 || 450-20 < playerNode.absolute.position.y + velocity.y)
    {
        velocity.y = 0;
    }
 
    playerNode.absolute.position.y += velocity.y;
    playerNode.absolute.position.x += velocity.x;
    if (true)
    {
        velocity.y *= 0.93;
        velocity.x *= 0.93;
    }
    auto rinf = createInfo(playerNode, playerTexture);
    //rinf.center = {11,8.5};
    //rinf.center = {0,0};
    //rinf.useCenter = true;
    entityRenderer1.addRenderTarget(rinf);
    clip.y -= 1;
    //playerNode.absolute.rotation += 1;
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
    entityRenderer1.render();
    SDL_RenderCopy(renderer, entityRenderer1.target, nullptr, &mid.absoluteLayout);
    //repeatTexture(renderer, spaceFull, {-clip.x, -clip.y}, dest, scale);
    //SDL_RenderCopy(renderer, spaceFull, &clip, &dest);
    //SDL_SetRenderDrawColor(renderer, 255, 0,0, 225);
    //SDL_RenderFillRect(renderer, &midline.absoluteLayout);
}

void SpaceGame::clear()
{
    entityRenderer1.clear();
}

void SpaceGame::applyResolution(int x, int y)
{
    lrender1.context.applyResolution(x, y);
    internalUnits = {lrender1.context.absoluteLayout.w, lrender1.context.absoluteLayout.h};
    lrender1.context.setInternalUnits({(int)(internalUnits.x*scale),(int)(internalUnits.y*scale)});
    //entityRenderer1.context.applyResolution(x,y);
    mid.applyResolution(x,y);
}
