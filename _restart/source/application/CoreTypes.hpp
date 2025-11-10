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

struct OrthoCamera {
    glm::mat4 view{1.0f}, proj{1.0f}; // build with glm::ortho(...)
};

struct QuadCmd {
    glm::mat4 mvp{1.0f};
    TextureHandle tex{};
    glm::vec4 color{1,1,1,1};
    glm::vec4 uv{0,0,1,1};     // atlas region
    uint32_t   sortKey{};      // layer/material for batching
};

struct RectCmd 
{
    TextureHandle tex{};
    glm::vec4 color{1,1,1,1};
};

struct RenderSnapshot2D {
    OrthoCamera cam;
    std::vector<QuadCmd> quads;
    std::function<void()> render;
};

using RenderSnapshots2D = std::vector<RenderSnapshot2D>;
struct EngineConfig { int w=1280, h=720; const char* title="Blink2D"; const char* version="1.0"; const char* id="example"; bool vsync=true; };

struct GameCallbacks {
    std::function<void(float, const std::span<const EventData>& )> tick; // dt, input, output
    std::function<void(RenderSnapshots2D&)> extract; // on-screen render
};
