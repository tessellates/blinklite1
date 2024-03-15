
#include "ConnectGame.hpp"
#include "BLApplication.hpp"
#include "TextureUtility.hpp"
#include "MovementComponent.hpp"

void ConnectGame::init()
{
    float pixelSize = 32;
    int pint = 32;
    resolutions = {{360,360}, {720, 720}, {1080,1080}};
    auto display = BLApplication::currentDisplay();
    windowContext.setFrameLayout({0.5, 0.5, 1, 1}, display.w*2, display.h*2);
    gameRenderer.context.setFrameLayout({0.5, 0.5, 1, 1}, pixelSize*7, pixelSize*7);
    gameRenderer.renderer = BLApplication::activeRenderer();
    gameRenderer.target = SDL_CreateTexture(gameRenderer.renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, pint*7, pint*7);
    gameRenderer.textureManager.addTexture(CreateTextureFromFile(gameRenderer.renderer, "assets/sdlbackdrop.png"));
    
    windowContext.setInternalUnits({pint*7, pint*7});
    gameRenderer.context.setInternalUnits({pint*7, pint*7});
    Rect gridRect = {0,16,pixelSize*7,pixelSize*6};
    grid = Grid(gridRect, 7, 6);
    connectGui.init(grid);
    connectGui.gameRenderer = &gameRenderer;
    connectModel = ConnectModel();
    connectModel.addConnectEntity.push_back([&](const Coordinate& p, int c) {this->connectGui.addConnectEntity(p, c);});
    connectModel.removeConnectEntity.push_back([&](const Coordinate& p) {this->connectGui.removeConnectEntity(p);});
    connectModel.changeConnectEntity.push_back([&](const Coordinate& p, int c) {this->connectGui.changeConnectEntity(p, c);});

    gameRenderer.textureManager.addTexture(CreateTextureFromFile(gameRenderer.renderer, "assets/rcoin.png"));
    gameRenderer.textureManager.addTexture(CreateTextureFromFile(gameRenderer.renderer, "assets/gcoin.png"));
    gameRenderer.textureManager.addTexture(CreateColorTexture(gameRenderer.renderer, SDL_MapRGBA(SDL_AllocFormat(SDL_PIXELFORMAT_RGBA8888), 255, 255, 0, 180)));
    auto indexRed = gameRenderer.textureManager.addTexture(CreateColorTexture(gameRenderer.renderer, SDL_MapRGBA(SDL_AllocFormat(SDL_PIXELFORMAT_RGBA8888), 255, 0, 0, 80)));
    auto indexGreen = gameRenderer.textureManager.addTexture(CreateColorTexture(gameRenderer.renderer, SDL_MapRGBA(SDL_AllocFormat(SDL_PIXELFORMAT_RGBA8888), 255, 0, 0, 80)));
    //SDL_SetTextureBlendMode(gameRenderer.textureManager.textures[indexRed], SDL_BLENDMODE_NONE);
    //SDL_SetTextureBlendMode(gameRenderer.textureManager.textures[indexGreen], SDL_BLENDMODE_NONE);
    gameRenderer.textureManager.addTexture(CreateColorTexture(gameRenderer.renderer, SDL_MapRGBA(SDL_AllocFormat(SDL_PIXELFORMAT_RGBA8888), 255, 255, 50, 180)));
    SDL_Texture* tile = CreateTextureFromFile(gameRenderer.renderer, "assets/node.png");
    SDL_Texture* tile2 = CreateTextureFromFile(gameRenderer.renderer, "assets/nodef.png");
    gameRenderer.textureManager.addTexture(CreateGridTexture(gameRenderer.renderer, tile, 7, 6));
    gameRenderer.textureManager.addTexture(CreateTextureFromFile(gameRenderer.renderer, "assets/TOP.png"));
    gameRenderer.textureManager.addTexture(CreateGridTexture(gameRenderer.renderer, tile2, 7, 6));
    
    background.dest = gridRect;
    background.layerID = 0;
    background.textureID = 7;
    foreground = background;
    foreground.layerID = 3;
    foreground.textureID = 9;
    top.dest = {4,4,63,5};
    top.layerID = 2;
    top.textureID = 8;
    bot.dest = {4,63,63,5};
    bot.layerID = 2;
    bot.textureID = 8;
    bot.flip = SDL_FLIP_VERTICAL;

    side1.dest = {67,5,62,2};
    side1.layerID = 2;
    side1.textureID = 8;
    side1.rotation = 90;
    side2 = side1;
    side2.rotation = 90;
    side2.dest = {2,5,62,2};
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
                auto point = windowContext.pointInUnits({mouseX, mouseY});
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
    auto point = windowContext.pointInUnits({mouseX, mouseY});
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
    /*
    gameRenderer.addRenderTarget(background);
    gameRenderer.addRenderTarget(top);
    gameRenderer.addRenderTarget(bot);
    gameRenderer.addRenderTarget(side1);
    gameRenderer.addRenderTarget(side2);
    gameRenderer.addRenderTarget(foreground);
    */
    gameRenderer.addRenderTarget(background);
    gameRenderer.addRenderTarget(foreground);
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
   
void ConnectGame::render() 
{
    gameRenderer.render();
    SDL_RenderCopy(gameRenderer.renderer, gameRenderer.target, nullptr, &windowContext.absoluteLayout);
}

void ConnectGame::clear()
{
    gameRenderer.clear();
}

void ConnectGame::applyResolution(int x, int y)
{
    windowContext.applyResolution(x,y);
}
