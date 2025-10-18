#pragma once
#include <functional>
#include <memory>
#include "CoreTypes.hpp"
#include "Event.hpp" 
#include <span>
#include "Singleton.hpp"

class TextureManager;
class QuadRenderer;

class Engine : public Singleton<Engine>{
    friend class Singleton<Engine>;
public:
    // boiler-plate singleton
    Engine(Engine &other) = delete;
    void operator=(const Engine &) = delete;
    
    bool init(const EngineConfig& config);
    void quit();
    void iterate(double dt, std::span<const EventData> events);
    
    void setCallbacks(const GameCallbacks& callbacks) { this->callbacks = callbacks; }
    void toggleFullscreen();

    glm::ivec2 getWindowSize() const;
    
    // Resource management
    TextureHandle loadBMPTexture(const char* path);
    TextureHandle loadBMPTextureWithKey(const char* path, const glm::ivec3& colorKey);
    
    // Renderer access (for ImGui)
    void* getRenderer();
    
    bool isFullscreen = false;
    
private:
    Engine();
    ~Engine();

    void render(const RenderSnapshot2D& snapshot);
    void handleEngineEvents(std::span<const EventData> events);
    
    // Proper PIMPL with smart pointer
    struct Impl;
    std::unique_ptr<Impl> pimpl;
    
    GameCallbacks callbacks;
    RenderSnapshots2D renderSnapshots;
    
    // Proper resource management
    std::unique_ptr<TextureManager> textureManager;
    std::unique_ptr<QuadRenderer> quadRenderer;
};