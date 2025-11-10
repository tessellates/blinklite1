#pragma once
#include "CRTP.hpp"
#include "CoreTypes.hpp"
#include "RenderPipeline.hpp"
#include <memory>

template<typename Derived>
class RenderBackend : public CRTP<Derived> {
public:
    bool init(const EngineConfig& config) {
        pipeline = std::make_unique<RenderPipeline>();
        return this->derived().initImpl(config);
    }
    
    void shutdown() {
        pipeline.reset();
        this->derived().shutdownImpl();
    }
    
    void beginFrame() {
        this->derived().beginFrameImpl();
    }
    
    void endFrame() {
        this->derived().endFrameImpl();
    }
    
    // High-level API - accepts blink render snapshots
    void render(const std::vector<RenderSnapshot2D>& snapshots) {
        this->derived().renderImpl(snapshots);
    }
    
    TextureHandle loadTexture(const char* path) {
        return this->derived().loadTextureImpl(path);
    }
    
    glm::ivec2 getWindowSize() const {
        return this->derived().getWindowSizeImpl();
    }
    
    void* getNativeRenderer() {
        return this->derived().getNativeRendererImpl();
    }

    void toggleFullscreen() {
        this->derived().toggleFullscreenImpl();
    }


private:
    std::unique_ptr<RenderPipeline> pipeline;
};