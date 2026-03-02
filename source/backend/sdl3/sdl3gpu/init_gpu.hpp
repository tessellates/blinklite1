#pragma once

#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL.h>

#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlgpu3.h"


struct GPUContext {
    SDL_GPUDevice* device = nullptr;
    SDL_Window* window = nullptr;
};

void initImguiGPUBackend(GPUContext* gpuContext) {
    ImGui_ImplSDL3_InitForSDLGPU(gpuContext->window);
    ImGui_ImplSDLGPU3_InitInfo init_info = {};
    init_info.Device = gpu_device;
    init_info.ColorTargetFormat = SDL_GetGPUSwapchainTextureFormat(gpu_device, window);
    init_info.MSAASamples = SDL_GPU_SAMPLECOUNT_1;
    ImGui_ImplSDLGPU3_Init(&init_info);

}