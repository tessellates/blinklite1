// types2d.hpp
#pragma once
#include <cstdint>
#include <vector>
#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include <span>
#include "Event.hpp"
#include <functional>

using TextureHandle = uint32_t;

struct QuadCmd {
    glm::vec2 position{0, 0};    // World position
    glm::vec2 size{1, 1};        // Width/height
    float rotation{0.0f};        // Rotation in radians
    TextureHandle tex{0};        // 0 = no texture (colored quad)
    glm::vec4 color{1,1,1,1};    // Color/tint
    glm::vec4 uv{0,0,1,1};       // UV coordinates (ignored if tex == 0)
    uint32_t sortKey{0};
};

struct RenderContext2D {
    glm::vec4 viewport{0, 0, 1280, 720}; // x, y, width, height
    // Can add viewMatrix, projectionMatrix later when needed
};


struct RenderSnapshot2D {
    RenderContext2D context;
    std::vector<QuadCmd> quads;  // Single unified quad list
    std::function<void(void*)> customRender;
};

using RenderSnapshots2D = std::vector<RenderSnapshot2D>;
struct EngineConfig { int w=1280, h=720; const char* title="Blink2D"; const char* version="1.0"; const char* id="example"; bool vsync=true; };

struct GameCallbacks {
    std::function<void(const EventData&)> onEvent; // input events
    std::function<void(float)> tick; // dt
    std::function<void(RenderSnapshots2D&)> extract; // on-screen render
};
