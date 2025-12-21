#pragma once

#include <SDL3/SDL_gpu.h>
#include <fstream>
#include <vector>
#include <array>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_iostream.h>
#include <cstring>
#include <limits>
/*
SDL_GPUComputePipeline* CreateComputePipelineFromShader(
    SDL_GPUDevice* device,
    const char* shaderFilename,
    SDL_GPUComputePipelineCreateInfo *createInfo
    ) {
    char fullPath[256];
    SDL_GPUShaderFormat backendFormats = SDL_GetGPUShaderFormats(device);
    SDL_GPUShaderFormat format = SDL_GPU_SHADERFORMAT_INVALID;
    const char *entrypoint;

    if (backendFormats & SDL_GPU_SHADERFORMAT_SPIRV) {
        SDL_snprintf(fullPath, sizeof(fullPath), "%sContent/Shaders/Compiled/SPIRV/%s.spv", BasePath, shaderFilename);
        format = SDL_GPU_SHADERFORMAT_SPIRV;
        entrypoint = "main";
    } else if (backendFormats & SDL_GPU_SHADERFORMAT_MSL) {
        SDL_snprintf(fullPath, sizeof(fullPath), "%sContent/Shaders/Compiled/MSL/%s.msl", BasePath, shaderFilename);
        format = SDL_GPU_SHADERFORMAT_MSL;
        entrypoint = "main0";
    } else if (backendFormats & SDL_GPU_SHADERFORMAT_DXIL) {
        SDL_snprintf(fullPath, sizeof(fullPath), "%sContent/Shaders/Compiled/DXIL/%s.dxil", BasePath, shaderFilename);
        format = SDL_GPU_SHADERFORMAT_DXIL;
        entrypoint = "main";
    } else {
        SDL_Log("%s", "Unrecognized backend shader format!");
        return NULL;
    }

    size_t codeSize;
    void* code = SDL_LoadFile(fullPath, &codeSize);
    if (code == NULL)
    {
        SDL_Log("Failed to load compute shader from disk! %s", fullPath);
        return NULL;
    }

    // Make a copy of the create data, then overwrite the parts we need
    SDL_GPUComputePipelineCreateInfo newCreateInfo = *createInfo;
    newCreateInfo.code = code;
    newCreateInfo.code_size = codeSize;
    newCreateInfo.entrypoint = entrypoint;
    newCreateInfo.format = format;

    SDL_GPUComputePipeline* pipeline = SDL_CreateGPUComputePipeline(device, &newCreateInfo);
    if (pipeline == NULL)
    {
        SDL_Log("Failed to create compute pipeline!");
        SDL_free(code);
        return NULL;
    }

    SDL_free(code);
    return pipeline;
}*/
inline SDL_GPUBuffer* gTriangleVertexBuffer = nullptr;
inline SDL_GPUBufferBinding gTriangleVertexBinding = {};
inline std::array<float, 6> gTriangleVertices = {
    -0.5f, -0.5f,
     0.5f, -0.5f,
     0.0f,  0.6f
};

inline constexpr Uint32 gTriangleVertexCount = 3;

inline SDL_GPUTexture* gOffscreenRenderTexture = nullptr;
inline SDL_GPUTransferBuffer* gReadbackTransferBuffer = nullptr;
inline Uint32 gRenderWidth = 0;
inline Uint32 gRenderHeight = 0;
inline Uint32 gSDLTextureWidth = 0;
inline Uint32 gSDLTextureHeight = 0;

inline void DestroyStagingGPUResources(SDL_GPUDevice* device)
{
    if (device == nullptr)
    {
        return;
    }

    if (gTriangleVertexBuffer != nullptr)
    {
        SDL_ReleaseGPUBuffer(device, gTriangleVertexBuffer);
        gTriangleVertexBuffer = nullptr;
    }

    if (gOffscreenRenderTexture != nullptr)
    {
        SDL_ReleaseGPUTexture(device, gOffscreenRenderTexture);
        gOffscreenRenderTexture = nullptr;
    }

    if (gReadbackTransferBuffer != nullptr)
    {
        SDL_ReleaseGPUTransferBuffer(device, gReadbackTransferBuffer);
        gReadbackTransferBuffer = nullptr;
    }

    gRenderWidth = 0;
    gRenderHeight = 0;
    gSDLTextureWidth = 0;
    gSDLTextureHeight = 0;
}

inline bool EnsureOffscreenTargets(SDL_GPUDevice* device, SDL_Window* window)
{
    if (device == nullptr || window == nullptr)
    {
        return false;
    }

    int width = 0;
    int height = 0;
    SDL_GetWindowSizeInPixels(window, &width, &height);
    if (width <= 0 || height <= 0)
    {
        return false;
    }

    const Uint32 desiredWidth = static_cast<Uint32>(width);
    const Uint32 desiredHeight = static_cast<Uint32>(height);

    if (gOffscreenRenderTexture != nullptr &&
        gReadbackTransferBuffer != nullptr &&
        desiredWidth == gRenderWidth &&
        desiredHeight == gRenderHeight)
    {
        return true;
    }

    if (gOffscreenRenderTexture != nullptr)
    {
        SDL_ReleaseGPUTexture(device, gOffscreenRenderTexture);
        gOffscreenRenderTexture = nullptr;
    }
    if (gReadbackTransferBuffer != nullptr)
    {
        SDL_ReleaseGPUTransferBuffer(device, gReadbackTransferBuffer);
        gReadbackTransferBuffer = nullptr;
    }

    const Uint64 bytesRequired = static_cast<Uint64>(desiredWidth) * static_cast<Uint64>(desiredHeight) * 4u;
    if (bytesRequired > std::numeric_limits<Uint32>::max())
    {
        SDL_Log("Requested render target is too large for transfer buffer (size=%llu).",
            static_cast<unsigned long long>(bytesRequired));
        return false;
    }

    SDL_GPUTextureCreateInfo textureInfo = {};
    textureInfo.type = SDL_GPU_TEXTURETYPE_2D;
    textureInfo.format = SDL_GetGPUSwapchainTextureFormat(device, window);
    textureInfo.usage = SDL_GPU_TEXTUREUSAGE_COLOR_TARGET | SDL_GPU_TEXTUREUSAGE_SAMPLER;
    textureInfo.width = desiredWidth;
    textureInfo.height = desiredHeight;
    textureInfo.layer_count_or_depth = 1;
    textureInfo.num_levels = 1;
    textureInfo.sample_count = SDL_GPU_SAMPLECOUNT_1;

    gOffscreenRenderTexture = SDL_CreateGPUTexture(device, &textureInfo);
    if (gOffscreenRenderTexture == nullptr)
    {
        SDL_Log("Failed to create offscreen render texture: %s", SDL_GetError());
        return false;
    }

    SDL_GPUTransferBufferCreateInfo downloadInfo = {};
    downloadInfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_DOWNLOAD;
    downloadInfo.size = static_cast<Uint32>(bytesRequired);
    gReadbackTransferBuffer = SDL_CreateGPUTransferBuffer(device, &downloadInfo);
    if (gReadbackTransferBuffer == nullptr)
    {
        SDL_Log("Failed to create GPU download buffer: %s", SDL_GetError());
        SDL_ReleaseGPUTexture(device, gOffscreenRenderTexture);
        gOffscreenRenderTexture = nullptr;
        return false;
    }

    gRenderWidth = desiredWidth;
    gRenderHeight = desiredHeight;
    return true;
}

static bool CreateTriangleGeometry(SDL_GPUDevice* device)
{
    std::cout << "Creating triangle geometry buffer..." << std::endl;
    if (gTriangleVertexBuffer != nullptr)
    {
        return true;
    }

    const Uint32 vertexBufferSize = static_cast<Uint32>(sizeof(float) * gTriangleVertices.size());

    SDL_GPUBufferCreateInfo bufferInfo = {};
    bufferInfo.usage = SDL_GPU_BUFFERUSAGE_VERTEX | SDL_GPU_BUFFERUSAGE_GRAPHICS_STORAGE_READ;
    bufferInfo.size = vertexBufferSize;
    gTriangleVertexBuffer = SDL_CreateGPUBuffer(device, &bufferInfo);
    if (gTriangleVertexBuffer == nullptr)
    {
        SDL_Log("Failed to create triangle vertex buffer: %s", SDL_GetError());
        return false;
    }

    SDL_GPUTransferBufferCreateInfo transferInfo = {};
    transferInfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
    transferInfo.size = vertexBufferSize;
    SDL_GPUTransferBuffer* transferBuffer = SDL_CreateGPUTransferBuffer(device, &transferInfo);
    if (transferBuffer == nullptr)
    {
        SDL_Log("Failed to create triangle transfer buffer: %s", SDL_GetError());
        SDL_ReleaseGPUBuffer(device, gTriangleVertexBuffer);
        gTriangleVertexBuffer = nullptr;
        return false;
    }

    void* mappedData = SDL_MapGPUTransferBuffer(device, transferBuffer, true);
    if (mappedData == nullptr)
    {
        SDL_Log("Failed to map triangle transfer buffer: %s", SDL_GetError());
        SDL_ReleaseGPUTransferBuffer(device, transferBuffer);
        SDL_ReleaseGPUBuffer(device, gTriangleVertexBuffer);
        gTriangleVertexBuffer = nullptr;
        return false;
    }
    std::memcpy(mappedData, gTriangleVertices.data(), vertexBufferSize);
    SDL_UnmapGPUTransferBuffer(device, transferBuffer);

    SDL_GPUCommandBuffer* copyCmd = SDL_AcquireGPUCommandBuffer(device);
    SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(copyCmd);

    SDL_GPUTransferBufferLocation sourceLocation = {};
    sourceLocation.transfer_buffer = transferBuffer;
    sourceLocation.offset = 0;

    SDL_GPUBufferRegion destinationRegion = {};
    destinationRegion.buffer = gTriangleVertexBuffer;
    destinationRegion.offset = 0;
    destinationRegion.size = vertexBufferSize;

    SDL_UploadToGPUBuffer(copyPass, &sourceLocation, &destinationRegion, false);
    SDL_EndGPUCopyPass(copyPass);
    SDL_SubmitGPUCommandBuffer(copyCmd);

    SDL_ReleaseGPUTransferBuffer(device, transferBuffer);

    gTriangleVertexBinding.buffer = gTriangleVertexBuffer;
    gTriangleVertexBinding.offset = 0;

    return true;
}

inline void initGPU(SDL_GPUDevice*& device, SDL_Window*& window, SDL_GPUGraphicsPipeline*& FillPipeline, SDL_GPUGraphicsPipeline*& LinePipeline)
{
    SDL_GPUShaderFormat formats = SDL_GPU_SHADERFORMAT_METALLIB | SDL_GPU_SHADERFORMAT_MSL;
    device = SDL_CreateGPUDevice( formats, false, NULL);
    if (device == NULL) {
        SDL_Log("SDL_CreateGPUDevice failed: %s", SDL_GetError());
        return;
    }
    SDL_Log("Using %s GPU implementation.",SDL_GetGPUDeviceDriver(device));
      // Then bind the window and GPU device together
    if (!SDL_ClaimWindowForGPUDevice(device, window)) {
        SDL_Log("SDL_ClaimWindowForGPUDevice failed: %s",
            SDL_GetError());
        return;
    }


    std::string fullPath = "assets/rect.metallib";
    
	size_t codeSize;
    void* code = SDL_LoadFile(fullPath.c_str(), &codeSize);
    SDL_GPUShaderCreateInfo vertexShaderInfo = {
        .code = static_cast<Uint8*>(code),
        .code_size = codeSize,
        .entrypoint = "vs_main",
        .format = SDL_GPU_SHADERFORMAT_METALLIB,
        .stage = SDL_GPU_SHADERSTAGE_VERTEX,
        .num_samplers = 0,
        .num_uniform_buffers = 0,
        .num_storage_buffers = 1,
        .num_storage_textures = 0
    };

    SDL_GPUShaderCreateInfo fragmentShaderInfo = {
        .code = static_cast<Uint8*>(code),
        .code_size = codeSize,
        .entrypoint = "fs_main",
        .format = SDL_GPU_SHADERFORMAT_METALLIB,
        .stage = SDL_GPU_SHADERSTAGE_FRAGMENT,
        .num_samplers = 0,
        .num_uniform_buffers = 0,
        .num_storage_buffers = 0,
        .num_storage_textures = 0
    };

    SDL_GPUShader* vertexShader = SDL_CreateGPUShader(device, &vertexShaderInfo);
    SDL_GPUShader* fragmentShader = SDL_CreateGPUShader(device, &fragmentShaderInfo);



	// Create the pipelines
	SDL_GPUColorTargetDescription colorTargetDescriptions[1] = {{
		.format = SDL_GetGPUSwapchainTextureFormat(device, window)
	}};

	SDL_GPUVertexAttribute vertexAttributes[1] = {};
	vertexAttributes[0].buffer_slot = 0;
	vertexAttributes[0].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2;
	vertexAttributes[0].location = 0;
	vertexAttributes[0].offset = 0;

	SDL_GPUVertexBufferDescription vertexBufferDescriptions[1] = {};
	vertexBufferDescriptions[0].slot = 0;
	vertexBufferDescriptions[0].input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX;
	vertexBufferDescriptions[0].instance_step_rate = 0;
	vertexBufferDescriptions[0].pitch = sizeof(float) * 2;

	SDL_GPUVertexInputState vertexInputState = {};
	vertexInputState.num_vertex_attributes = 1;
	vertexInputState.vertex_attributes = vertexAttributes;
	vertexInputState.num_vertex_buffers = 1;
	vertexInputState.vertex_buffer_descriptions = vertexBufferDescriptions;

	SDL_GPUGraphicsPipelineCreateInfo pipelineCreateInfo = {
		.target_info = {
			.num_color_targets = 1,
			.color_target_descriptions = colorTargetDescriptions,
		},
		.vertex_input_state = vertexInputState,
		.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST,
		.vertex_shader = vertexShader,
		.fragment_shader = fragmentShader,
	};

	pipelineCreateInfo.rasterizer_state.fill_mode = SDL_GPU_FILLMODE_FILL;
	FillPipeline = SDL_CreateGPUGraphicsPipeline(device, &pipelineCreateInfo);
	if (FillPipeline == NULL)
	{
		SDL_Log("Failed to create fill pipeline!");
		return;
	}

	pipelineCreateInfo.rasterizer_state.fill_mode = SDL_GPU_FILLMODE_LINE;
	LinePipeline = SDL_CreateGPUGraphicsPipeline(device, &pipelineCreateInfo);
	if (LinePipeline == NULL)
	{
		SDL_Log("Failed to create line pipeline!");
		return;
	}

	if (!CreateTriangleGeometry(device))
	{
		SDL_Log("Failed to create triangle geometry buffer!");
		return;
	}

    
	// Clean up shader resources
	SDL_ReleaseGPUShader(device, vertexShader);
	SDL_ReleaseGPUShader(device, fragmentShader);

	// Finally, print instructions!
	SDL_Log("Press Left to toggle wireframe mode");
	SDL_Log("Press Down to toggle small viewport");
	SDL_Log("Press Right to toggle scissor rect");


    std::cout << "GPU Device initialized." << std::endl;
}

inline void renderWithCustomPipeline(SDL_GPUDevice* device,
	SDL_Window* window,
	SDL_GPUGraphicsPipeline*& FillPipeline,
	SDL_GPUGraphicsPipeline*& LinePipeline,
	SDL_Renderer* renderer,
	SDL_Texture*& outputTexture)
{
	(void)LinePipeline;
	if (device == nullptr || window == nullptr || renderer == nullptr)
	{
		return;
	}

	if (!EnsureOffscreenTargets(device, window))
	{
		return;
	}

	SDL_GPUCommandBuffer* cmdBuf = SDL_AcquireGPUCommandBuffer(device);
	SDL_GPUColorTargetInfo colorTargetInfo = {};
	colorTargetInfo.texture = gOffscreenRenderTexture;
	colorTargetInfo.clear_color = (SDL_FColor){ 1.0f, 1.0f, 1.0f, 1.0f };
	colorTargetInfo.load_op = SDL_GPU_LOADOP_CLEAR;
	colorTargetInfo.store_op = SDL_GPU_STOREOP_STORE;

	SDL_GPURenderPass* renderPass = SDL_BeginGPURenderPass(cmdBuf, &colorTargetInfo, 1, NULL);
	SDL_BindGPUGraphicsPipeline(renderPass, FillPipeline);

	SDL_GPUViewport viewport = {};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.w = static_cast<float>(gRenderWidth);
	viewport.h = static_cast<float>(gRenderHeight);
	viewport.min_depth = 0.0f;
	viewport.max_depth = 1.0f;
	SDL_SetGPUViewport(renderPass, &viewport);

	SDL_Rect scissor = { 0, 0, static_cast<int>(gRenderWidth), static_cast<int>(gRenderHeight) };
	SDL_SetGPUScissor(renderPass, &scissor);

	if (gTriangleVertexBuffer == nullptr)
	{
		SDL_Log("Triangle vertex buffer is not ready, skipping draw.");
	}
	else
	{
		SDL_GPUBuffer* vertexStorageBuffers[1] = { gTriangleVertexBuffer };
		SDL_BindGPUVertexStorageBuffers(renderPass, 0, vertexStorageBuffers, 1);
		SDL_BindGPUVertexBuffers(renderPass, 0, &gTriangleVertexBinding, 1);
		SDL_DrawGPUPrimitives(renderPass, gTriangleVertexCount, 1, 0, 0);
	}
	SDL_EndGPURenderPass(renderPass);

	SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(cmdBuf);
	SDL_GPUTextureRegion sourceRegion = {};
	sourceRegion.texture = gOffscreenRenderTexture;
	sourceRegion.mip_level = 0;
	sourceRegion.layer = 0;
	sourceRegion.x = 0;
	sourceRegion.y = 0;
	sourceRegion.z = 0;
	sourceRegion.w = gRenderWidth;
	sourceRegion.h = gRenderHeight;
	sourceRegion.d = 1;

	SDL_GPUTextureTransferInfo destinationInfo = {};
	destinationInfo.transfer_buffer = gReadbackTransferBuffer;
	destinationInfo.offset = 0;
	destinationInfo.pixels_per_row = gRenderWidth;
	destinationInfo.rows_per_layer = gRenderHeight;

	SDL_DownloadFromGPUTexture(copyPass, &sourceRegion, &destinationInfo);
	SDL_EndGPUCopyPass(copyPass);

	SDL_SubmitGPUCommandBuffer(cmdBuf);
	SDL_WaitForGPUIdle(device);

	void* mappedData = SDL_MapGPUTransferBuffer(device, gReadbackTransferBuffer, false);
	if (mappedData == nullptr)
	{
		SDL_Log("Failed to map GPU download buffer: %s", SDL_GetError());
		return;
	}

	const bool textureNeedsResize = outputTexture == nullptr ||
		gRenderWidth != gSDLTextureWidth ||
		gRenderHeight != gSDLTextureHeight;
	if (textureNeedsResize)
	{
        auto props = SDL_GetTextureProperties(gOffscreenRenderTexture);
		if (outputTexture != nullptr)
		{
			SDL_DestroyTexture(outputTexture);
			outputTexture = nullptr;
		}
        std::cout << "Creating SDL texture of size: " << gRenderWidth << "x" << gRenderHeight << std::endl;
		outputTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING,
			static_cast<int>(gRenderWidth), static_cast<int>(gRenderHeight));
		if (outputTexture == nullptr)
		{
			SDL_Log("Failed to create SDL texture for GPU output: %s", SDL_GetError());
			SDL_UnmapGPUTransferBuffer(device, gReadbackTransferBuffer);
			return;
		}
		SDL_SetTextureBlendMode(outputTexture, SDL_BLENDMODE_BLEND);
		gSDLTextureWidth = gRenderWidth;
		gSDLTextureHeight = gRenderHeight;
	}

	const Uint32 rowBytes = gRenderWidth * 4;
	if (SDL_UpdateTexture(outputTexture, nullptr, mappedData, static_cast<int>(rowBytes)) != 0)
	{
        float tw, th;
        SDL_GetTextureSize(outputTexture, &tw, &th);
        std::cout << "Updating texture size to: " << tw << "x" << th << std::endl;
		SDL_Log("Failed to update SDL texture for GPU output: %s", SDL_GetError());
		SDL_UnmapGPUTransferBuffer(device, gReadbackTransferBuffer);
		return;
	}

	SDL_UnmapGPUTransferBuffer(device, gReadbackTransferBuffer);
}

    // And finally, submit the command buffer for drawing. The
    // driver will take over at this point and do all the rendering
    // we've asked it to.
    /*  
    std::ifstream f("rect.metallib", std::ios::binary);
    std::vector<uint8_t> blob((std::istreambuf_iterator<char>(f)), {});
    SDL_GPUSwapchain* swapchain = SDL_CreateGPUSwapchain(gpu, window);
    SDL_GPUShaderCreateInfo si{};
    si.format = SDL_GPU_SHADERFORMAT_METALLIB;
    si.code = blob.data();
    si.code_size = blob.size();

    si.stage = SDL_GPU_SHADERSTAGE_VERTEX;
    si.entrypoint = "vs_main";
    SDL_GPUShader* vs = SDL_CreateGPUShader(gpu, &si);

    si.stage = SDL_GPU_SHADERSTAGE_FRAGMENT;
    si.entrypoint = "fs_main";
    SDL_GPUShader* fs = SDL_CreateGPUShader(gpu, &si);

    SDL_GPUGraphicsPipelineCreateInfo pi{};
    pi.vertex_shader   = vs;
    pi.fragment_shader = fs;
    pi.primitive_type  = SDL_GPU_PRIMITIVETYPE_TRIANGLESTRIP;
    SDL_GPUGraphicsPipeline* pipe = SDL_CreateGPUGraphicsPipeline(gpu, &pi);

    // --- DRAW ---
    SDL_GPUTexture* bb;
    SDL_AcquireGPUSwapchainTexture(gpu, swapchain, &bb, nullptr, nullptr);
    SDL_GPUCommandBuffer* cb = SDL_AcquireGPUCommandBuffer(gpu);

    SDL_GPUColorTargetInfo ct{};
    ct.texture = bb;
    ct.load_op = SDL_GPU_LOADOP_CLEAR;
    ct.store_op = SDL_GPU_STOREOP_STORE;
    ct.clear_color = {0,0,0,1};

    SDL_GPURenderPassCreateInfo rp{};
    rp.color_target_count = 1;
    rp.color_targets = &ct;

    SDL_GPURenderPass* pass = SDL_BeginGPURenderPass(cb, &rp);
    SDL_BindGPUGraphicsPipeline(pass, pipe);
    SDL_DrawGPUPrimitives(pass, 4, 1, 0, 0);
    SDL_EndGPURenderPass(pass);

    SDL_SubmitGPUCommandBuffer(cb);
}
    
    */
