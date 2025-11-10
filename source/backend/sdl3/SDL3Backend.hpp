#pragma once
#include "RenderBackend.hpp"
#include <SDL3/SDL.h>
#include <memory>
#include "CoreTypes.hpp"

class TextureManager;

class SDL3Backend : public RenderBackend<SDL3Backend> {
    friend class RenderBackend<SDL3Backend>;
    
public:
    SDL3Backend();
    ~SDL3Backend();
    
    // CRTP implementation methods
    bool initImpl(const EngineConfig& config);
    void shutdownImpl();
    void beginFrameImpl();
    void renderImpl(const std::vector<RenderSnapshot2D>& snapshots);
    void endFrameImpl();
    void toggleFullscreenImpl();
    TextureHandle loadTextureImpl(const char* path);
    glm::ivec2 getWindowSizeImpl() const;
    void* getNativeRendererImpl();
    
private:

    void renderQuad(const QuadCmd& quad);
    void renderColoredQuad(const QuadCmd& quad);
    void renderTexturedQuad(const QuadCmd& quad);

    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    std::unique_ptr<TextureManager> textureManager;
    bool running = false;
};