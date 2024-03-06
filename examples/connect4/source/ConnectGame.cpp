
#include "ConnectGame.hpp"
#include "BLApplication.hpp"
#include "CreateTexture.hpp"

void ConnectGame::init()
{
    gameRenderer = &BLApplication::mainRenderer();
    int gridU = 2000;
    gameRenderer->setInternalUnits({gridU, gridU});
    grid = Grid({gridU/72, gridU/12, (int)((70.0/72.0)*gridU), (int)((60.0/72.0)*gridU)}, 7, 6);
    connectGui.init(grid);
    connectGui.gameRenderer = gameRenderer;
    connectModel = ConnectModel();
    connectModel.addConnectEntity.push_back([&](const Coordinate& p, int c) {this->connectGui.addConnectEntity(p, c);});
    connectModel.removeConnectEntity.push_back([&](const Coordinate& p) {this->connectGui.removeConnectEntity(p);});
    connectModel.changeConnectEntity.push_back([&](const Coordinate& p, int c) {this->connectGui.changeConnectEntity(p, c);});

    gameRenderer->textureManager.addTexture(CreateColorTexture(gameRenderer->sdlRenderer, SDL_MapRGBA(SDL_AllocFormat(SDL_PIXELFORMAT_RGBA8888), 255, 0, 0, 255)));
    gameRenderer->textureManager.addTexture(CreateColorTexture(gameRenderer->sdlRenderer, SDL_MapRGBA(SDL_AllocFormat(SDL_PIXELFORMAT_RGBA8888), 0, 255, 0, 255)));
    gameRenderer->textureManager.addTexture(CreateColorTexture(gameRenderer->sdlRenderer, SDL_MapRGBA(SDL_AllocFormat(SDL_PIXELFORMAT_RGBA8888), 255, 255, 0, 255)));
    gameRenderer->textureManager.addTexture(CreateColorTexture(gameRenderer->sdlRenderer, SDL_MapRGBA(SDL_AllocFormat(SDL_PIXELFORMAT_RGBA8888), 255, 0, 0, 80)));
    gameRenderer->textureManager.addTexture(CreateColorTexture(gameRenderer->sdlRenderer, SDL_MapRGBA(SDL_AllocFormat(SDL_PIXELFORMAT_RGBA8888), 0, 255, 0, 80)));
    gameRenderer->textureManager.addTexture(CreateColorTexture(gameRenderer->sdlRenderer, SDL_MapRGBA(SDL_AllocFormat(SDL_PIXELFORMAT_RGBA8888), 255, 255, 0, 80)));
}

void ConnectGame::handleEvent(const SDL_Event& event)
{
    switch (event.type) 
    {
        case SDL_MOUSEMOTION: {
            int mouseX = event.button.x;
            int mouseY = event.button.y;
            auto point = gameRenderer->pointInUnits({mouseX, mouseY});
            if (point.x >= 0)
            {
                if (SDL_PointInRect(&point, &grid.rect))
                {
                    hover(grid.coordinate(point));
                    return;
                }
            }
            connectModel.resetCycle();
            break;
        }
        case SDL_MOUSEBUTTONDOWN: {
            if (event.button.button == SDL_BUTTON_LEFT) 
            {
                int mouseX = event.motion.x;
                int mouseY = event.motion.y;
                auto point = gameRenderer->pointInUnits({mouseX, mouseY});
                if (point.x >= 0)
                {
                    if (SDL_PointInRect(&point, &grid.rect))
                    {
                        clicked(grid.coordinate(point));
                    }
                }
            }
            break;
        }
        case SDL_KEYDOWN: {
            if (event.key.keysym.sym == SDLK_LEFT)
            {
                backward();
            }
            if (event.key.keysym.sym == SDLK_RIGHT)
            {
                forward();
            }
            break;
        }
    }
}

void ConnectGame::clicked(const Coordinate& position)
{
    play = false;
    connectModel.resetCycle();
    connectModel.play(position.x);
    currentPreview = -1;
    play = true;
}

void ConnectGame::hover(const Coordinate& position)
{
    if (currentPreview != position.x)
    {
        connectModel.resetCycle();
        currentPreview = position.x;
        connectGui.previewMode = true;
        connectModel.preview(position.x);
        connectGui.previewMode = false;
    }
}

void ConnectGame::run()
{
    connectGui.render();
}

void ConnectGame::forward()
{
    connectModel.forward();
}

void ConnectGame::backward()
{
    connectModel.backward();
}
   