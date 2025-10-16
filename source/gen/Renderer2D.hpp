#pragma once

#include "RenderCommands.hpp"
#include <memory>

namespace BL::Render {

/**
 * Abstract interface for the rendering backend.
 * This allows SDL3, OpenGL, Vulkan, or any other backend to be plugged in.
 */
class IRenderBackend {
public:
    virtual ~IRenderBackend() = default;
    
    // Resource management
    virtual TextureHandle createTexture(const void* data, int width, int height, int channels = 4) = 0;
    virtual void destroyTexture(TextureHandle handle) = 0;
    virtual ShaderHandle createShader(const char* vertexSource, const char* fragmentSource) = 0;
    virtual void destroyShader(ShaderHandle handle) = 0;
    virtual BufferHandle createBuffer(const void* data, size_t size) = 0;
    virtual void destroyBuffer(BufferHandle handle) = 0;
    
    // Frame rendering
    virtual void beginFrame(const Viewport& viewport) = 0;
    virtual void setCamera(const Camera2D& camera) = 0;
    virtual void executeCommands(const std::span<const RenderCommand>& commands) = 0;
    virtual void endFrame() = 0;
    
    // Utility
    virtual void setVSync(bool enabled) = 0;
    virtual glm::ivec2 getBackbufferSize() const = 0;
};

/**
 * High-level renderer that manages the render frame and delegates to backend
 */
class Renderer2D {
public:
    explicit Renderer2D(std::unique_ptr<IRenderBackend> backend);
    ~Renderer2D();
    
    // Resource management (forwarded to backend)
    TextureHandle loadTexture(const char* path);
    TextureHandle createTexture(const void* data, int width, int height, int channels = 4);
    void destroyTexture(TextureHandle handle);
    
    ShaderHandle createShader(const char* vertexSource, const char* fragmentSource);
    void destroyShader(ShaderHandle handle);
    
    // Frame management
    void beginFrame();
    void setCamera(const Camera2D& camera);
    void setViewport(const Viewport& viewport);
    
    // Command submission (high-level API)
    void drawTexture(TextureHandle texture, const Transform2D& transform, 
                     const Color& tint = Color::White(), const UV& uv = UV::Full());
    void drawRect(const glm::vec4& rect, const Color& color, 
                  const Transform2D& transform = {});
    void drawRect(const glm::vec2& position, const glm::vec2& size, const Color& color,
                  const Transform2D& transform = {});
    void drawRectOutline(const glm::vec4& rect, const Color& color, float borderWidth = 1.0f,
                        const Transform2D& transform = {});
    void drawCircle(const Circle& circle, const Color& color,
                   const Transform2D& transform = {});
    void drawLine(const Line& line, const Color& color,
                 const Transform2D& transform = {});
    
    // Low-level command submission
    void submit(const RenderCommand& command);
    void submit(const TextureCommand& command);
    void submit(const RectCommand& command);
    void submit(const CircleCommand& command);
    void submit(const LineCommand& command);
    void submit(const CustomCommand& command);
    
    // Frame execution
    void endFrame();
    
    // Utility
    void setVSync(bool enabled);
    glm::ivec2 getBackbufferSize() const;
    
    // Direct access to render frame for advanced usage
    RenderFrame& getCurrentFrame() { return currentFrame; }
    const RenderFrame& getCurrentFrame() const { return currentFrame; }
    
private:
    std::unique_ptr<IRenderBackend> backend;
    RenderFrame currentFrame;
    uint32_t nextSortKey = 0;
};

} // namespace BL::Render