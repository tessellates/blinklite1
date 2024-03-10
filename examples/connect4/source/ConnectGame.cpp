
#include "ConnectGame.hpp"
#include "BLApplication.hpp"
#include "TextureUtility.hpp"
#include "MovementComponent.hpp"

void ConnectGame::init()
{
    resolutions = {{360,360}, {720, 720}, {1080,1080}};
    gameRenderer = &BLApplication::mainRenderer();
    //int gridU = 72;
    gameRenderer->setInternalUnits({72, 72});
    gameRenderer->toggleTextureLayerMode(true);

    grid = Grid({4.5,9,63,54}, 7, 6);
    connectGui.init(grid);
    connectGui.gameRenderer = gameRenderer;
    connectModel = ConnectModel();
    connectModel.addConnectEntity.push_back([&](const Coordinate& p, int c) {this->connectGui.addConnectEntity(p, c);});
    connectModel.removeConnectEntity.push_back([&](const Coordinate& p) {this->connectGui.removeConnectEntity(p);});
    connectModel.changeConnectEntity.push_back([&](const Coordinate& p, int c) {this->connectGui.changeConnectEntity(p, c);});

    gameRenderer->textureManager.addTexture(CreateTextureFromFile(gameRenderer->renderer, "assets/rcoin.png"));
    gameRenderer->textureManager.addTexture(CreateTextureFromFile(gameRenderer->renderer, "assets/gcoin.png"));
    gameRenderer->textureManager.addTexture(CreateColorTexture(gameRenderer->renderer, SDL_MapRGBA(SDL_AllocFormat(SDL_PIXELFORMAT_RGBA8888), 255, 255, 0, 180)));
    gameRenderer->textureManager.addTexture(CreateColorTexture(gameRenderer->renderer, SDL_MapRGBA(SDL_AllocFormat(SDL_PIXELFORMAT_RGBA8888), 255, 0, 0, 80)));
    gameRenderer->textureManager.addTexture(CreateColorTexture(gameRenderer->renderer, SDL_MapRGBA(SDL_AllocFormat(SDL_PIXELFORMAT_RGBA8888), 0, 255, 0, 80)));
    gameRenderer->textureManager.addTexture(CreateColorTexture(gameRenderer->renderer, SDL_MapRGBA(SDL_AllocFormat(SDL_PIXELFORMAT_RGBA8888), 255, 255, 50, 180)));
    SDL_Texture* tile = CreateTextureFromFile(gameRenderer->renderer, "assets/node.png");
    SDL_Texture* tile2 = CreateTextureFromFile(gameRenderer->renderer, "assets/nodef.png");
    gameRenderer->textureManager.addTexture(CreateGridTexture(gameRenderer->renderer, tile, 7, 6));
    gameRenderer->textureManager.addTexture(CreateTextureFromFile(gameRenderer->renderer, "assets/TOP.png"));
    gameRenderer->textureManager.addTexture(CreateGridTexture(gameRenderer->renderer, tile2, 7, 6));
    background.dest = {4.5,9,63,54};
    background.layerID = 0;
    background.textureID = 7;
    foreground = background;
    foreground.layerID = 3;
    foreground.textureID = 9;
    top.dest = {4.5,4,63,5};
    top.layerID = 2;
    top.textureID = 8;
    bot.dest = {4.5,63,63,5};
    bot.layerID = 2;
    bot.textureID = 8;
    bot.flip = SDL_FLIP_VERTICAL;

    side1.dest = {67.5,5,62,2};
    side1.layerID = 2;
    side1.textureID = 8;
    side1.rotation = 90;
    side2 = side1;
    side2.rotation = 90;
    side2.dest = {2.5,5,62,2};
    side2.flip = SDL_FLIP_VERTICAL;
}

void ConnectGame::handleEvent(const SDL_Event& event)
{
    switch (event.type) 
    {
        case SDL_MOUSEMOTION: {
            int mouseX = event.motion.x;
            int mouseY = event.motion.y;
            hover(mouseX, mouseY);
            break;
        }
        case SDL_MOUSEBUTTONDOWN: {
            if (event.button.button == SDL_BUTTON_LEFT) 
            {
                int mouseX = event.button.x;
                int mouseY = event.button.y;
                auto point = gameRenderer->pointInUnits({mouseX, mouseY});
                if (point.x >= 0)
                {
                    if (pointInRect(point, grid.rect))
                    {
                        clicked(grid.coordinate(point));
                        hover(mouseX, mouseY);
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

void ConnectGame::hover(int mouseX, int mouseY)
{
    auto point = gameRenderer->pointInUnits({mouseX, mouseY});
    if (point.x >= 0)
    {
        if (pointInRect(point, grid.rect))
        {
            auto position = grid.coordinate(point);
            if (currentPreview != position.x)
            {
                connectModel.resetCycle();
                currentPreview = position.x;
                connectGui.previewMode = true;
                connectModel.preview(position.x);
                connectGui.previewMode = false;
            }
            return;
        }
    }
    currentPreview = -1;
    connectModel.resetCycle();
}

void ConnectGame::run()
{
    gameRenderer->addRenderTarget(background);
    gameRenderer->addRenderTarget(top);
    gameRenderer->addRenderTarget(bot);
    gameRenderer->addRenderTarget(side1);
    gameRenderer->addRenderTarget(side2);
    gameRenderer->addRenderTarget(foreground);
    connectGui.render();
}

void ConnectGame::forward()
{
    connectModel.resetCycle();
    currentPreview = -1;
    connectModel.forward();
    int x,y;
    SDL_GetMouseState(&x, &y);
    hover(x*2, y*2);
}

void ConnectGame::backward()
{
    connectModel.resetCycle();
    currentPreview = -1;

    connectModel.backward();
    
    int x,y;
    SDL_GetMouseState(&x, &y);
    hover(x*2, y*2);
}
   