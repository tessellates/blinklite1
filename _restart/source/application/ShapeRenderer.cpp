#include "ShapeRenderer.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>
#include <cmath>

ShapeRenderer::ShapeRenderer(TextureManager& texManager) 
    : textureManager(texManager) {
}

void ShapeRenderer::init() {
    // Create white pixel texture for shape rendering
    whitePixelTexture = textureManager.createWhitePixel();
}

void ShapeRenderer::addRect(RenderSnapshot2D& snapshot, const Rect& rect, const glm::vec4& color, 
                           uint32_t sortKey, const glm::mat4& transform) {
    QuadCmd quad = createRectQuad(rect, color, sortKey, transform);
    snapshot.quads.push_back(quad);
}

void ShapeRenderer::addRectOutline(RenderSnapshot2D& snapshot, const Rect& rect, const glm::vec4& color, 
                                  float borderWidth, uint32_t sortKey, const glm::mat4& transform) {
    // Create 4 rectangles for the border
    float x = rect.x, y = rect.y, w = rect.z, h = rect.w;
    
    // Top border
    Rect topBorder = {x, y, w, borderWidth};
    addRect(snapshot, topBorder, color, sortKey, transform);
    
    // Bottom border  
    Rect bottomBorder = {x, y + h - borderWidth, w, borderWidth};
    addRect(snapshot, bottomBorder, color, sortKey, transform);
    
    // Left border
    Rect leftBorder = {x, y + borderWidth, borderWidth, h - 2 * borderWidth};
    addRect(snapshot, leftBorder, color, sortKey, transform);
    
    // Right border
    Rect rightBorder = {x + w - borderWidth, y + borderWidth, borderWidth, h - 2 * borderWidth};
    addRect(snapshot, rightBorder, color, sortKey, transform);
}

void ShapeRenderer::addCircle(RenderSnapshot2D& snapshot, const Circle& circle, const glm::vec4& color, 
                             uint32_t sortKey, uint32_t segments, const glm::mat4& transform) {
    std::vector<QuadCmd> circleQuads = createCircleQuads(circle, color, sortKey, segments, transform);
    for (const auto& quad : circleQuads) {
        snapshot.quads.push_back(quad);
    }
}

void ShapeRenderer::addLine(RenderSnapshot2D& snapshot, const Line& line, const glm::vec4& color, 
                           uint32_t sortKey, const glm::mat4& transform) {
    QuadCmd quad = createLineQuad(line, color, sortKey, transform);
    snapshot.quads.push_back(quad);
}

QuadCmd ShapeRenderer::createRectQuad(const Rect& rect, const glm::vec4& color, uint32_t sortKey, const glm::mat4& transform) const {
    QuadCmd cmd;
    cmd.tex = whitePixelTexture; // Use white pixel for colored shapes
    cmd.color = color;
    cmd.sortKey = sortKey;
    cmd.uv = {0, 0, 1, 1}; // Full texture (white pixel)
    
    // Create MVP matrix: transform * translation * scale
    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(rect.x, rect.y, 0.0f));
    model = glm::scale(model, glm::vec3(rect.z, rect.w, 1.0f));
    cmd.mvp = transform * model;
    
    return cmd;
}

std::vector<QuadCmd> ShapeRenderer::createCircleQuads(const Circle& circle, const glm::vec4& color, 
                                                     uint32_t sortKey, uint32_t segments, const glm::mat4& transform) const {
    std::vector<QuadCmd> quads;
    
    // Create circle as triangle fan using quads
    // For simplicity, we'll approximate with many small rectangles
    float angleStep = 2.0f * glm::pi<float>() / segments;
    
    for (uint32_t i = 0; i < segments; ++i) {
        float angle1 = i * angleStep;
        float angle2 = (i + 1) * angleStep;
        
        // Create a small triangle/quad for this segment
        glm::vec2 p1 = circle.center + circle.radius * glm::vec2(std::cos(angle1), std::sin(angle1));
        glm::vec2 p2 = circle.center + circle.radius * glm::vec2(std::cos(angle2), std::sin(angle2));
        
        // Create a small quad between center and edge
        glm::vec2 midPoint = (p1 + p2) * 0.5f;
        glm::vec2 toMid = midPoint - circle.center;
        float segmentWidth = glm::length(p2 - p1);
        
        Rect segmentRect = {
            circle.center.x + toMid.x * 0.5f - segmentWidth * 0.5f,
            circle.center.y + toMid.y * 0.5f - 1.0f,
            segmentWidth,
            glm::length(toMid)
        };
        
        QuadCmd quad = createRectQuad(segmentRect, color, sortKey, transform);
        quads.push_back(quad);
    }
    
    return quads;
}

QuadCmd ShapeRenderer::createLineQuad(const Line& line, const glm::vec4& color, uint32_t sortKey, const glm::mat4& transform) const {
    // Convert line to rotated rectangle
    glm::vec2 delta = line.end - line.start;
    float length = glm::length(delta);
    float angle = std::atan2(delta.y, delta.x);
    
    glm::vec2 center = (line.start + line.end) * 0.5f;
    
    QuadCmd cmd;
    cmd.tex = whitePixelTexture;
    cmd.color = color;
    cmd.sortKey = sortKey;
    cmd.uv = {0, 0, 1, 1};
    
    // Create MVP: translate to center, rotate, scale to line dimensions
    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(center, 0.0f));
    model = glm::rotate(model, angle, glm::vec3(0, 0, 1));
    model = glm::scale(model, glm::vec3(length, line.width, 1.0f));
    model = glm::translate(model, glm::vec3(-0.5f, -0.5f, 0.0f)); // Center the quad
    
    cmd.mvp = transform * model;
    
    return cmd;
}