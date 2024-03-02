#include "BLApplication.hpp"
#include "BlinkGame.hpp"
#include "BlinkMenu.hpp"
#include "TextureUtility.hpp"

#include <iostream>
#include <SDL_image.h>
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"
#include "SDLUtil.hpp"


void toggleFullscreen(SDL_Window* window) {
    Uint32 fullscreenFlag = SDL_WINDOW_FULLSCREEN;
    Uint32 isFullscreen = SDL_GetWindowFlags(window) & fullscreenFlag;
    SDL_SetWindowFullscreen(window, isFullscreen ? 0 : fullscreenFlag);
}

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
    // From 2.0.18: Enable native IME.
    #ifdef SDL_HINT_IME_SHOW_UI
    SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");
    #endif
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
                //io_->DisplaySize = ImVec2((float)450, (float)450);
            }
            if (event.key.keysym.sym == SDLK_b)
            {
                toggleFullscreen(window);
            }
        }
    }

    SDL_RenderClear(renderer);

    if (game != nullptr)
    {
        game->run();
    }
    else
    {
        // Set up the destination rectangle
        SDL_Rect dest_rect {0, 0};
        dest_rect.w = blRenderer.externals().x;
        dest_rect.h = blRenderer.externals().y;
        RenderInfo info {};
        info.dest = dest_rect;
        info.textureID = 0;
        info.layerID = 1;
        blRenderer.addRenderTarget(info);
        blRenderer.render();
    }
    SDL_SetRenderDrawColor(renderer, 50,0,0,255);

    // Start the Dear ImGui frame
    if (imguiToggle)
    {
        ImGui_ImplSDLRenderer2_NewFrame();
        ImGui_ImplSDL2_NewFrame();

        //io_->DisplayFramebufferScale = ImVec2(1.0f, 1.0f);
        //io_->DisplaySize = ImVec2((float)x_resolution, (float)y_resolution);

        //io_->MousePos.x *= std::ceil((float)x_resolution/float(display.w));
        //io_->MousePos.y *= std::ceil((float)y_resolution/float(display.h));
        //io_->MousePos.y *= std::ceil(pViewport->DpiScale);
        ImGui::NewFrame();
        //io_->DisplaySize = ImVec2((float)450, (float)450);
  		//ImGui::ShowDemoWindow();

        menu->run();
        ImGui::Render();
        SDL_RenderSetScale(renderer, io_->DisplayFramebufferScale.x, io_->DisplayFramebufferScale.y);
        blRenderer.setInternalContext({(int)((x_resolution - y_resolution)/2/io_->DisplayFramebufferScale.x), 0, (int)(y_resolution/io_->DisplayFramebufferScale.x), (int)(y_resolution/io_->DisplayFramebufferScale.y)});
        //SDL_RenderClear(renderer);
        //ImGui::GetDrawData()->FramebufferScale  = {2,2};
        ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());
    }
    else
    {
        SDL_RenderSetScale(renderer, 1, 1);
        blRenderer.setInternalContext({(x_resolution - y_resolution)/2, 0, y_resolution, y_resolution});
    }

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

    float baseDpi = 108.5f; // Adjust this base DPI to your needs
    float ddpi, hdpi, vdpi;
    SDL_GetDisplayDPI(0, &ddpi, &hdpi, &vdpi); // Assuming displayIndex 0 for simplicity
    float scaleFactor = ddpi / baseDpi;
    std::cout << scaleFactor << std::endl;
    std::cout << ddpi << std::endl;
    std::cout << hdpi << std::endl;
    std::cout << vdpi << std::endl;
    if (window_flags & SDL_WINDOW_ALLOW_HIGHDPI)
    {
        std::cout << "HIGH DPI" << std::endl;
        x_resolution = display.w*scaleFactor;
        y_resolution = display.h*scaleFactor;
    }
    else
    {
        x_resolution = display.w;
        y_resolution = display.h;
    }

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
    blRenderer.sdlRenderer = renderer;
    int xpos = (x_resolution - y_resolution)/2;
    blRenderer.setInternalContext({xpos, 0, y_resolution, y_resolution});
    blRenderer.setExternalContext({900,900});
    menu = new BlinkMenu();
    if (test)
        blRenderer.textureManager.addTexture(CreateTextureFromFile(renderer, "assets/test.png"));
    //SDL_RenderSetLogicalSize(renderer, x_resolution, y_resolution);
    //io_->DisplaySize = ImVec2({(float)x_resolution, (float)y_resolution}) / io_->DisplayFramebufferScale;
    //ImGui::GetStyle().ScaleAllSizes(1.5);
}

void BLApplication::init(BlinkGame* blinkGame)
{
    if(!isInit)
    {
        init();
    }
    game = blinkGame;
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

BLSDLRenderer& BLApplication::mainRenderer()
{
    return BLApplication::instance()->blRenderer;
}

const SDL_Rect& BLApplication::currentDisplay()
{
    return BLApplication::instance()->display;
}

void BLApplication::changeDisplay(const ImVec2& dimensions)
{
    auto& app = *BLApplication::instance();

    float modx = dimensions.x/app.display.w;
    float mody = dimensions.y/app.display.h;

    app.display.h = dimensions.y;
    app.display.w = dimensions.x;

    auto inter = app.blRenderer.internals();
    multiply_rect_x(inter, modx);
    multiply_rect_y(inter, mody);
    app.blRenderer.setInternalContext(inter);
}
