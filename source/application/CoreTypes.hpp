// types2d.hpp
#pragma once
#include <cstdint>
#include <vector>
#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include <span>
#include "Action.hpp"

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

struct RenderSnapshot2D {
    OrthoCamera cam;
    std::vector<QuadCmd> quads;
};

struct EngineConfig { int w=1280, h=720; const char* title="Blink2D"; const char* version="1.0"; const char* id="example"; bool vsync=true; };

struct GameCallbacks {
    std::function<void(float, std::span<const Actions>, RenderSnapshot2D&)> step; // dt, input, output
    std::function<void(const RenderSnapshot2D&)> render; // on-screen render
};