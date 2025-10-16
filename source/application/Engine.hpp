#pragma once
#include <functional>
#include <memory>
#include "CoreTypes.hpp"
#include "Event.hpp" 
#include <span>

// Forward declarations - no SDL in headers!
class TextureManager;
class QuadRenderer;

/**
 * FIXED ENGINE - Proper separation of concerns
 */
class Engine {
public:
    Engine();
    ~Engine();
    
    // Non-copyable
    Engine(const Engine&) = delete;
    Engine& operator=(const Engine&) = delete;
    
    bool init(const EngineConfig& config);
    void quit();
    void iterate(double dt, std::span<const EventData> events);
    
    void setCallbacks(const GameCallbacks& callbacks) { this->callbacks = callbacks; }
    void toggleFullscreen();
    
    // Resource management
    TextureHandle loadBMPTexture(const char* path);
    TextureHandle loadBMPTextureWithKey(const char* path, const glm::ivec3& colorKey);
    
    // Renderer access (for ImGui)
    void* getRenderer();
    
    bool isFullscreen = false;
    
private:
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