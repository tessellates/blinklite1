#pragma once
#include "Singleton.hpp"
#include "CoreTypes.hpp"
#include "RenderBackend.hpp"
#include <memory>
#include <span>

template<typename BackendType>
class EngineImpl : public Singleton<EngineImpl<BackendType>> {
    friend class Singleton<EngineImpl<BackendType>>;
    
private:
    EngineImpl() = default;
    ~EngineImpl() { } //quit(); }
    
public:
    bool init(const EngineConfig& config) {
        return backend.init(config);
    }
    
    void quit() {
        backend.shutdown();
    }

    void onEvent(const EventData& event)
    {
        handleEngineEvents(event);
        callbacks.onEvent(event);
    }

    void iterate(double dt) 
    {
        // Game tick
        if (callbacks.tick) {
            callbacks.tick(static_cast<float>(dt));
        }
        
        if (callbacks.extract) {
            callbacks.extract(renderSnapshots);
        }
        
        // Backend handles the pipeline conversion
        backend.beginFrame();
        backend.render(renderSnapshots); 
        backend.endFrame();
    }
    
    void setCallbacks(const GameCallbacks& callbacks) {
        this->callbacks = callbacks;
    }
    
    TextureHandle loadTexture(const char* path) {
        return backend.loadTexture(path);
    }
    
    glm::ivec2 getWindowSize() const {
        return backend.getWindowSize();
    }
    
    void* getRenderer() {
        return backend.getNativeRenderer();
    }

    RenderBackend<BackendType>& getBackend() {
        return backend;
    }

    void toggleFullscreen() {
        isFullscreen = !isFullscreen;
        backend.toggleFullscreenImpl();
    }
    
    bool isFullscreen = false;

private:
    void handleEngineEvents(const EventData& event) {
        switch (event.action) {
            case Event::FullscreenToggle:
                backend.toggleFullscreen();
                break;
            case Event::Quit:
                backend.shutdown();
                break;
            default:
                break;
        }
    }
    
    RenderBackend<BackendType> backend;  // Contains pipeline internally
    GameCallbacks callbacks;
    std::vector<RenderSnapshot2D> renderSnapshots;
};