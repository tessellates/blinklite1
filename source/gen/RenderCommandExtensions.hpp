#pragma once
#include "CoreTypes.hpp"
#include <variant>

namespace BL {

// Extend your existing system with additional command types
// Keep your excellent QuadCmd as the core!

// Primitive shape commands that convert to QuadCmds internally
struct RectCmd {
    glm::vec4 rect;           // x, y, w, h (GLM as you wanted)
    glm::vec4 color{1,1,1,1};
    float borderWidth = 0.0f; // 0 = filled, >0 = outline
    glm::vec4 borderColor{1,1,1,1};
    uint32_t sortKey = 0;
    
    // Convert to QuadCmd(s) - filled rect + border quads
    std::vector<QuadCmd> toQuadCommands(const glm::mat4& mvp) const;
};

struct CircleCmd {
    glm::vec2 center;
    float radius;
    glm::vec4 color{1,1,1,1};
    float borderWidth = 0.0f;
    glm::vec4 borderColor{1,1,1,1};
    uint32_t segments = 32;
    uint32_t sortKey = 0;
    
    // Convert to QuadCmd triangle fan
    std::vector<QuadCmd> toQuadCommands(const glm::mat4& mvp, TextureHandle whiteTexel) const;
};

struct LineCmd {
    glm::vec2 start, end;
    float width = 1.0f;
    glm::vec4 color{1,1,1,1};
    uint32_t sortKey = 0;
    
    // Convert to QuadCmd (line as rotated rectangle)
    QuadCmd toQuadCommand(const glm::mat4& mvp, TextureHandle whiteTexel) const;
};

// Advanced GPU command for custom shaders/meshes
struct CustomCmd {
    // Your existing QuadCmd handles most cases
    // This is for truly custom GPU work
    uint32_t vertexBuffer;
    uint32_t indexBuffer;
    uint32_t shader;
    std::vector<uint8_t> uniformData;
    uint32_t sortKey = 0;
};

using ExtendedCommand = std::variant<QuadCmd, RectCmd, CircleCmd, LineCmd, CustomCmd>;

// High-level renderer that converts everything to your QuadCmd system
class ExtendedRenderer2D {
public:
    // Keep your excellent RenderSnapshot2D system!
    void beginSnapshot(const OrthoCamera& camera);
    void endSnapshot(std::function<void()> renderCallback = {});
    
    // High-level API that converts to QuadCmds
    void drawTexture(TextureHandle tex, const glm::mat4& mvp, 
                     const glm::vec4& color = {1,1,1,1}, 
                     const glm::vec4& uv = {0,0,1,1}, 
                     uint32_t sortKey = 0);
    
    void drawRect(const glm::vec4& rect, const glm::vec4& color, uint32_t sortKey = 0);
    void drawCircle(const glm::vec2& center, float radius, const glm::vec4& color, uint32_t sortKey = 0);
    void drawLine(const glm::vec2& start, const glm::vec2& end, float width, const glm::vec4& color, uint32_t sortKey = 0);
    
    // Direct QuadCmd submission (for performance-critical code)
    void submitQuad(const QuadCmd& cmd);
    
    // Get the snapshots for your existing engine
    RenderSnapshots2D& getSnapshots() { return snapshots; }
    
private:
    RenderSnapshots2D snapshots;
    RenderSnapshot2D* currentSnapshot = nullptr;
    TextureHandle whitePixelTexture = 0; // 1x1 white texture for shapes
};

// Builder pattern for your existing QuadCmd
class QuadBuilder {
public:
    QuadBuilder& texture(TextureHandle tex) { cmd.tex = tex; return *this; }
    QuadBuilder& mvp(const glm::mat4& m) { cmd.mvp = m; return *this; }
    QuadBuilder& color(const glm::vec4& c) { cmd.color = c; return *this; }
    QuadBuilder& uv(const glm::vec4& u) { cmd.uv = u; return *this; }
    QuadBuilder& uv(float u1, float v1, float u2, float v2) { 
        cmd.uv = {u1, v1, u2, v2}; return *this; 
    }
    QuadBuilder& sortKey(uint32_t key) { cmd.sortKey = key; return *this; }
    
    QuadCmd build() const { return cmd; }
    operator QuadCmd() const { return cmd; }
    
private:
    QuadCmd cmd;
};

// Convenience factory
inline QuadBuilder Quad() { return QuadBuilder(); }

} // namespace BL