#include "BLApplication.hpp"
#include "BlinkGame.hpp"
#include "TextureUtility.hpp"

#include <iostream>
#include <SDL_image.h>
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"

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
            if (event.key.keysym.sym == SDLK_b) {
                isBloom = !isBloom;
            }
        }
    }
    SDL_RenderClear(renderer);

    if (game != nullptr)
    {

    }
    else
    {
        int window_width, window_height;
        // Get window size
        SDL_GetWindowSize(window, &window_width, &window_height);
        // Set texture width and height to match the screen height
        int texture_width = window_height; // Texture's width is the same as the screen height
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
        if (isBloom)
        {
            SDL_SetRenderDrawColor(renderer, 0,0,0,0);
            int w, h;
            SDL_QueryTexture(testmod,NULL, NULL, &w, &h);
            Uint32 *upixels = (Uint32*)surfaceMod->pixels;
            std::vector<Uint32> pixelVector(upixels, upixels + w*h);
            applyBloomEffect(pixelVector, w, h);
            int pitch;
            void *pixels;
            SDL_LockTexture(testmod, NULL, &pixels, &pitch);
            std::memcpy(pixels, pixelVector.data(), pixelVector.size() * sizeof(Uint32));
            SDL_UnlockTexture(testmod);

            //SDL_RenderClear(renderer);
            /*
            std::cout << dest_rect.w << dest_rect.h << std::endl;
            SDL_QueryTexture(testmod,NULL, NULL, &w, &h);
            std::cout << w << h << std::endl;*/

            SDL_RenderCopy(renderer, testmod, nullptr, &dest_rect);

        }
        else
        {
            SDL_RenderCopy(renderer, test, nullptr, &dest_rect);
        }
    }
    SDL_SetRenderDrawColor(renderer, 0,0,0,0);

    // Start the Dear ImGui frame
    if (imguiToggle)
    {
        ImGui_ImplSDLRenderer2_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();
        ImGui::SetNextWindowBgAlpha(0.1f);
        ImGui::Begin("Hello, world!", nullptr);
        ImGui::Button("Look at this pretty button");
        ImGui::End();

        ImGui::Render();
        SDL_RenderSetScale(renderer, io_->DisplayFramebufferScale.x, io_->DisplayFramebufferScale.y);
        //SDL_RenderClear(renderer);
        ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());
    }

    SDL_RenderPresent(renderer);
}

void BLApplication::init()
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

    // Create window with SDL_Renderer graphics context
    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_FULLSCREEN_DESKTOP);
    window = SDL_CreateWindow("BLINKLITE", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 0, 0, window_flags);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
    std::cout << "blendmode:" << SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND) <<" -- " << SDL_BLENDMODE_BLEND << std::endl;

    if (renderer == nullptr)
    {
        SDL_Log("Error creating SDL_Renderer!");
    }

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    io_ = &ImGui::GetIO();
    //io_->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io_->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer2_Init(renderer);

    test = CreateTextureFromFile(renderer, "assets/sdlbackdrop.png");
    int w; int h;
    SDL_QueryTexture(test,NULL, NULL, &w, &h);
    testmod = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, w, h);
    surfaceMod = IMG_Load("assets/sdlbackdrop.png");
    surfaceMod = SDL_ConvertSurfaceFormat(surfaceMod, SDL_PIXELFORMAT_RGBA8888, 0);
}

void BLApplication::init(BlinkGame* blinkGame)
{

}

BLApplication::BLApplication() {}
BLApplication::~BLApplication()
{
    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
