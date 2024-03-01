#include "BLApplication.hpp"
#include "BlinkGame.hpp"
#include "BlinkMenu.hpp"
#include "TextureUtility.hpp"

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
        // Test texture
        int w, h;
        SDL_QueryTexture(blRenderer.textureManager.getTexture(0) ,NULL, NULL, &w, &h);
        int window_width, window_height;
        // Get window size
        window_width = display.w;
        window_height = display.h;
        //std::cout << window_width << " " << window_height << std::endl;
        // Set texture width and height to match the screen height
        int texture_width = window_height * w/h; // Texture's width is the same as the screen height
        int texture_height = window_height; // Texture's height
        // Calculate centered position
        int x_position = (window_width - texture_width) / 2;
        int y_position = (window_height - texture_height) / 2;

        // Set up the destination rectangle
        SDL_Rect dest_rect;
        dest_rect.x = x_position;
        dest_rect.y = y_position;
        dest_rect.w = texture_width;
        dest_rect.h = texture_height;


        RenderInfo info {};
        RenderInfo info2 {};
        info.dest = dest_rect;
        info.textureID = 0;
        info.layerID = 1;
        info2.textureID = 0;
        dest_rect.x = 0;
        dest_rect.y = 0;
        dest_rect.w = y_resolution;
        dest_rect.h = y_resolution;
        info2.dest = dest_rect;

        blRenderer.addRenderTarget(info);
        //blRenderer.addRenderTarget(info2);
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

  		ImGui::ShowDemoWindow();

        //menu->run();
        ImGui::Render();
        //SDL_RenderSetScale(renderer, io_->DisplayFramebufferScale.x, io_->DisplayFramebufferScale.y);
        //SDL_RenderClear(renderer);
        //ImGui::GetDrawData()->FramebufferScale  = {2,2};
        ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());
    }

    SDL_RenderPresent(renderer);
}

void BLApplication::init(bool test)
{
    if (SDL_GetDisplayBounds(0, &display) != 0)
    {
        std::cout <<  SDL_GetError()  << std::endl;
    }
    x_resolution = 800;
    y_resolution = 600;
    display.w = x_resolution;
    display.h = y_resolution;
    // Create window with SDL_Renderer graphics context
    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_FULLSCREEN_DESKTOP );//| SDL_WINDOW_ALLOW_HIGHDPI);
    window = SDL_CreateWindow("BLINKLITE", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, display.w, display.h, window_flags);
    std::cout << display.w << std::endl;
    std::cout << display.h << std::endl;
    //window = SDL_CreateWindow("BLINKLITE", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, x_resolution, y_resolution, window_flags);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);

    if (renderer == nullptr)
    {
        SDL_Log("Error creating SDL_Renderer!");
    }
    //x_resolution = 2560;
    //y_resolution = 1600;

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    io_ = &ImGui::GetIO();
    io_->IniFilename = NULL;
    io_->LogFilename = NULL;
   	//io_->DisplaySize.x = display.w;
	//io_->DisplaySize.y = display.h;
    //io_->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io_->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer2_Init(renderer);
    //io_->FontGlobalScale = 3; // Scale the fonts
    //ImGui::GetStyle().ScaleAllSizes(4);

    isInit = true;
    blRenderer.sdlRenderer = renderer;
    menu = new BlinkMenu();
    if (test)
        blRenderer.textureManager.addTexture(CreateTextureFromFile(renderer, "assets/az.png"));
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
