
#include "ConnectGame.hpp"
#include "BLApplication.hpp"
#include "TextureUtility.hpp"
#include "MovementComponent.hpp"

void ConnectGame::init()
{
    float pixelSize = 32;
    int pint = 32;
    float diff = 1.125;
    resolutions = {{768*diff,768}, {1344, 1152}, {1080*diff,1024}};
    auto display = BLApplication::currentDisplay();
    frameRenderer = LayeredRenderer();
    frameRenderer.context.frameLayout = {0.5, 0.5, 1, float(7)/6, CENTER, CENTER, true, {res*7, res*6}};
    gameRenderer = LayeredRenderer();
    gameRenderer.context.setFrameLayout({0.5, 0.5, 1, 1.16666666667}, res*7, res*6);
    
    gameRenderer.renderer = BLApplication::activeRenderer();
    gameRenderer.target = SDL_CreateTexture(gameRenderer.renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, res*7, res*6);
    gameRenderer.textureManager.addTexture(CreateTextureFromFile(gameRenderer.renderer, "assets/sdlbackdrop.png"));
    gameRenderer.backgroundColor = {222, 235, 212, 255};
    frameRenderer.context.setInternalUnits({pint*7, pint*6});
    frameRenderer.context.applyResolution(display.w, display.h);

    gameRenderer.context.setInternalUnits({pint*7, pint*6});

    Rect gridRect = {0,0,pixelSize*7,pixelSize*6};
    grid = Grid(gridRect, 7, 6);
    connectGui = ConnectGui();
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
    auto indexGreen = gameRenderer.textureManager.addTexture(CreateColorTexture(gameRenderer.renderer, SDL_MapRGBA(SDL_AllocFormat(SDL_PIXELFORMAT_RGBA8888), 0, 255, 0, 80)));
    gameRenderer.textureManager.addTexture(CreateColorTexture(gameRenderer.renderer, SDL_MapRGBA(SDL_AllocFormat(SDL_PIXELFORMAT_RGBA8888), 255, 255, 50, 180)));
    SDL_Texture* tile = CreateTextureFromFile(gameRenderer.renderer, "assets/node.png");
    SDL_Texture* tile2 = CreateTextureFromFile(gameRenderer.renderer, "assets/nodef.png");
    gameRenderer.textureManager.addTexture(CreateGridTexture(gameRenderer.renderer, tile, 7, 6));
    gameRenderer.textureManager.addTexture(CreateGridTexture(gameRenderer.renderer, tile2, 7, 6));
    background.dest = gridRect;
    background.layerID = 0;
    background.textureID = 7;
    foreground = background;
    foreground.layerID = 3;
    foreground.textureID = 8;

    frameRenderer.textureManager.addTexture(CreateTextureFromFile(gameRenderer.renderer, "assets/TOP.png"));
    frameRenderer.renderer = gameRenderer.renderer;
    top.dest = {0,-pixelSize,pixelSize*7,pixelSize};
    top.layerID = 2;
    top.textureID = 0;
    bot.dest = {0,pixelSize*6,pixelSize*7,pixelSize};
    bot.layerID = 2;
    bot.textureID = 0;
    bot.flip = SDL_FLIP_VERTICAL;
    side1.dest = {pixelSize*7,-pixelSize/2,pixelSize*7,pixelSize}; 
    side1.layerID = 2;
    side1.textureID = 0;
    side1.rotation = 90;
    side2 = side1;
    side2.rotation = 90;
    side2.dest = {-pixelSize,-pixelSize/2,pixelSize*7,pixelSize};
    side2.flip = SDL_FLIP_VERTICAL;
    frameRenderer.addRenderTarget(top);
    frameRenderer.addRenderTarget(bot);
    frameRenderer.addRenderTarget(side1);
    frameRenderer.addRenderTarget(side2);
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
                auto point = frameRenderer.context.pointInUnits({mouseX, mouseY});
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
            if (event.key.keysym.sym == SDLK_a)
            {
                res--;
                init();
            }
            if (event.key.keysym.sym == SDLK_z)
            {
                res++;
                init();
            }
            if (event.key.keysym.sym == SDLK_e)
            {
                res += 15;
                init();
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
    auto point = frameRenderer.context.pointInUnits({mouseX, mouseY});
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
    hover(x, y);
}

void ConnectGame::backward()
{
    connectModel.resetCycle();
    currentPreview = -1;

    connectModel.backward();
    
    int x,y;
    SDL_GetMouseState(&x, &y);
    hover(x, y);
}
   
void ConnectGame::render() 
{
    gameRenderer.render();
    frameRenderer.render();
    SDL_RenderCopy(gameRenderer.renderer, gameRenderer.target, nullptr, &frameRenderer.context.absoluteLayout);
}

void ConnectGame::clear()
{
    gameRenderer.clear();
}

void ConnectGame::applyResolution(int x, int y)
{
    frameRenderer.context.applyResolution(x,y);
}
