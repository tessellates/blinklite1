#pragma once
#include "CoreTypes.hpp"
#include "TextureManager.hpp"
#include <vector>
#include <glm/vec4.hpp>

// GLM-based shapes (as you requested)
using Rect = glm::vec4; // x, y, width, height

struct Circle {
    glm::vec2 center;
    float radius;
};

struct Line {
    glm::vec2 start, end;
    float width = 1.0f;
};

/**
 * High-level shape rendering that converts primitives to QuadCmds.
 * Integrates with your existing QuadCmd system seamlessly.
 */
class ShapeRenderer {
public:
    explicit ShapeRenderer(TextureManager& texManager);
    
    // Initialize with white pixel texture (needed for shapes)
    void init();
    
    // Add shapes to a render snapshot - they become QuadCmds
    void addRect(RenderSnapshot2D& snapshot, const Rect& rect, const glm::vec4& color, 
                 uint32_t sortKey = 0, const glm::mat4& transform = glm::mat4(1.0f));
    
    void addRectOutline(RenderSnapshot2D& snapshot, const Rect& rect, const glm::vec4& color, 
                        float borderWidth, uint32_t sortKey = 0, const glm::mat4& transform = glm::mat4(1.0f));
    
    void addCircle(RenderSnapshot2D& snapshot, const Circle& circle, const glm::vec4& color, 
                   uint32_t sortKey = 0, uint32_t segments = 32, const glm::mat4& transform = glm::mat4(1.0f));
    
    void addLine(RenderSnapshot2D& snapshot, const Line& line, const glm::vec4& color, 
                 uint32_t sortKey = 0, const glm::mat4& transform = glm::mat4(1.0f));
    
private:
    TextureManager& textureManager;
    TextureHandle whitePixelTexture = 0;
    
    // Convert shapes to QuadCmds
    QuadCmd createRectQuad(const Rect& rect, const glm::vec4& color, uint32_t sortKey, const glm::mat4& transform) const;
    std::vector<QuadCmd> createCircleQuads(const Circle& circle, const glm::vec4& color, uint32_t sortKey, 
                                          uint32_t segments, const glm::mat4& transform) const;
    QuadCmd createLineQuad(const Line& line, const glm::vec4& color, uint32_t sortKey, const glm::mat4& transform) const;
};

// Builder pattern for easy quad creation (integrates with your existing system)
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

// Convenience factory functions
inline QuadBuilder Quad() { return QuadBuilder(); }

// GLM utility functions for rects
inline glm::vec2 getRectPosition(const Rect& rect) { return {rect.x, rect.y}; }
inline glm::vec2 getRectSize(const Rect& rect) { return {rect.z, rect.w}; }
inline glm::vec2 getRectCenter(const Rect& rect) { return {rect.x + rect.z * 0.5f, rect.y + rect.w * 0.5f}; }
inline bool containsPoint(const Rect& rect, const glm::vec2& point) {
    return point.x >= rect.x && point.x <= rect.x + rect.z &&
           point.y >= rect.y && point.y <= rect.y + rect.w;
}