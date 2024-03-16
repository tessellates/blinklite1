#include "BLApplication.hpp"
#include "BlinkGame.hpp"
#include "BlinkMenu.hpp"
#include "TextureUtility.hpp"
#include "SDLUtil.hpp"

#include <iostream>
#include <SDL_image.h>
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"

BLApplication::BLApplication()
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
    {
        printf("Error: %s\n", SDL_GetError());
    }

    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags)) {
        // Handle error
    }

    // Initialize SDL_ttf
    if (TTF_Init() == -1) {
        std::cerr << "SDL_ttf initialization failed: " << TTF_GetError() << std::endl;
        SDL_Quit(); // Clean up SDL before exiting
    }
    // From 2.0.18: Enable native IME.
    #ifdef SDL_HINT_IME_SHOW_UI
    SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");
    #endif

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
}

int BLApplication::run()
{
    while(isRunning)
    {
        loop();
    }
    return 0;
}

void BLApplication::loop()
{
    SDL_SetRenderDrawColor(renderer, 0,0,0,0);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_RenderClear(renderer);

    while (SDL_PollEvent(&event))
    {
        ImGui_ImplSDL2_ProcessEvent(&event);
        if (event.type == SDL_QUIT)
            isRunning = false;
        if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
            isRunning = false;
        if (event.type == SDL_KEYDOWN)
        {
            if (event.key.keysym.sym == SDLK_ESCAPE) {
                imguiToggle = !imguiToggle;
                hasToggled = true;
            }
        }
        if (event.type == BL_RESOLUTIONCHANGE)
        {
            changeWindow();
            hasToggled = true;

        }
        if (event.type == BL_FULLSCREEN_TOGGLE)
        {
            toggleFullscreen();
            hasToggled = true;
        }
        if (!imguiToggle && game != nullptr)   
            game->handleEvent(event);
    }

    clock.update();
    BLApplication::deltaTime = clock.getDeltaTime();
    if (game != nullptr && !imguiToggle)
    {
        game->clear();
        game->run();
    }

    game->render();

    SDL_SetRenderDrawColor(renderer, 0,0,0,255);

    // Start the Dear ImGui frame
    if (imguiToggle)
    {
        ImGui_ImplSDLRenderer2_NewFrame();
        ImGui_ImplSDL2_NewFrame();  
 
        float x = (float)xResolution/display.w;
        float y = (float)yResolution/display.h;

        if (game != nullptr && hasToggled)
        {
            game->applyResolution(display.w, display.h); 
            hasToggled = false;
        }
        menu->applyResolution(display.w, display.h);
        
        ImGui::NewFrame();
        menu->run();
        ImGui::Render();
        SDL_RenderSetLogicalSize(renderer, display.w, display.h);
        
        ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());
    }
    else
    {
        if (hasToggled)
        {
            SDL_RenderSetLogicalSize(renderer, xResolution, yResolution);
            if (game)
                game->applyResolution(xResolution, yResolution); 
            hasToggled = false;
        }
    }
    frc.update();
    if (BLApplication::frameRate)
        frc.render();
    SDL_RenderPresent(renderer);
}

void BLApplication::init(bool test)
{
    if (SDL_GetDisplayBounds(0, &display) != 0)
    {
        std::cout <<  SDL_GetError()  << std::endl;
    }
    
    // Create window with SDL_Renderer graphics context
    SDL_WindowFlags window_flags =  (SDL_WindowFlags)(SDL_WINDOW_FULLSCREEN_DESKTOP | SDL_WINDOW_ALLOW_HIGHDPI);
    window = SDL_CreateWindow("BLINKLITE", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, display.w, display.h, window_flags);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);


    if (window_flags & SDL_WINDOW_ALLOW_HIGHDPI)
    {
        std::cout << "HIGH DPI" << std::endl;
        float ddpi, hdpi, vdpi;
        SDL_GetDisplayDPI(0, &ddpi, &hdpi, &vdpi); // Assuming displayIndex 0 for simplicity
        float baseDpi = 108.5f; // Adjust this base DPI to your needs
        scaleFactor = 1;//ddpi / baseDpi;
    }
    else
    {
        scaleFactor = 1;
    }

    xResolution = display.w*scaleFactor;
    yResolution = display.h*scaleFactor;

    const std::vector<std::pair<int, int>> commonResolutions = {
        {800, 600}, {1024, 768}, {1280, 720}, {1680, 945}, {1920, 1080}, {2048,1152}
    };

    for (const auto& res : commonResolutions) {
        if (res.first < display.w && res.second < display.h) {
            resolutions.push_back(res);
        }
    }
    BLApplication::currentResolution = resolutions.size() - 1;

    if (renderer == nullptr)
    {
        SDL_Log("Error creating SDL_Renderer!");
    }

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    io_ = &ImGui::GetIO();
    io_->IniFilename = NULL;
    io_->LogFilename = NULL;
    io_->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io_->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer2_Init(renderer);
    //io_->FontGlobalScale = 3; // Scale the fonts
    //ImGui::GetStyle().ScaleAllSizes(4);

    isInit = true;
    //blRenderer.renderer = renderer;
    menu = new BlinkMenu();
    menu->addResolutions(resolutions);
    if (test)
        //blRenderer.textureManager.addTexture(CreateTextureFromFile(renderer, "assets/sdlbackdrop.png"));

    //blRenderer.frameLayout = {0.5, 0.5, 1, 1};
    //blRenderer.applyResolution(xResolution, yResolution);

    std::cout << "STATS FOR CATS" << std::endl;
    std::cout << "xres:" << xResolution << std::endl;
    std::cout << "yres:" << yResolution <<std::endl;
    std::cout << "disx:" << display.w << std::endl;
    std::cout << "disy:" << display.h << std::endl;

    TTF_Font* font = TTF_OpenFont("assets/Arial.ttf", 24);
    if (!font) {
        std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
        // Handle error
    }
    frc = FrameRateCounter(renderer, font);
}

void BLApplication::init(BlinkGame* blinkGame)
{
    if(!isInit)
    {
        init();
    }
    game = blinkGame;
    game->init();
    menu->addResolutions(game->resolutions);

    resolutions.clear();
    for (const auto& res : game->resolutions) {
        if (res.first < display.w && res.second < display.h) {
            resolutions.push_back(res);
        }
    }
    BLApplication::currentResolution = resolutions.size() - 1;
}

//BLApplication::BLApplication() {}
BLApplication::~BLApplication()
{
    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

//STATIC
BlinkGame* BLApplication::activeGame()
{
    return BLApplication::instance()->game;
}

const SDL_Rect& BLApplication::currentDisplay()
{
    return BLApplication::instance()->display;
}

SDL_Renderer* BLApplication::activeRenderer()
{
    return BLApplication::instance()->renderer;
}


void BLApplication::changeWindow(const std::pair<int,int>& size)
{
    auto& app = *BLApplication::instance();
    SDL_SetWindowSize(app.window, size.first, size.second);
    app.correctDisplay();
}

void BLApplication::changeWindow()
{
    SDL_SetWindowSize(window, resolutions[BLApplication::currentResolution].first, resolutions[BLApplication::currentResolution].second);
    correctDisplay();
}

void BLApplication::correctDisplay()
{
    SDL_GetWindowSize(window,&display.w, &display.h);
    if (display.h % 2)
    {
        display.h--;
        SDL_SetWindowSize(window, display.w, display.h);
    }

    xResolution = display.w*scaleFactor;
    yResolution = display.h*scaleFactor;
}

void BLApplication::toggleFullscreen() 
{
    Uint32 fullscreenFlag = SDL_WINDOW_FULLSCREEN_DESKTOP;
    Uint32 isFullscreen = SDL_GetWindowFlags(window) & fullscreenFlag;
    SDL_SetWindowFullscreen(window, isFullscreen ? 0 : fullscreenFlag);
    BLApplication::isFullscreen = !isFullscreen;
    if (!BLApplication::isFullscreen)
    {
        BLApplication::changeWindow(resolutions[BLApplication::currentResolution]);
    }
    else
    {
        correctDisplay();
    }
}

bool BLApplication::isFullscreen = true;
bool BLApplication::frameRate = true;
int BLApplication::currentResolution = 0;
float BLApplication::deltaTime = 0;