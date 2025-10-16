#pragma once

#include <cstdint>
#include <variant>
#include <vector>
#include <span>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

namespace BL::Render {

// Forward declarations
using TextureHandle = uint32_t;
using ShaderHandle = uint32_t;
using BufferHandle = uint32_t;

// Basic geometric types using GLM
using Rect = glm::vec4; // x, y, width, height

struct Circle {
    glm::vec2 center;
    float radius;
};

struct Line {
    glm::vec2 start, end;
    float width = 1.0f;
};

// Color and material properties
struct Color {
    float r, g, b, a;
    
    constexpr Color(float r = 1.0f, float g = 1.0f, float b = 1.0f, float a = 1.0f) 
        : r(r), g(g), b(b), a(a) {}
    
    static constexpr Color White() { return {1, 1, 1, 1}; }
    static constexpr Color Black() { return {0, 0, 0, 1}; }
    static constexpr Color Red() { return {1, 0, 0, 1}; }
    static constexpr Color Green() { return {0, 1, 0, 1}; }
    static constexpr Color Blue() { return {0, 0, 1, 1}; }
    static constexpr Color Transparent() { return {0, 0, 0, 0}; }
};

// UV coordinates for texture sampling
struct UV {
    glm::vec2 min{0.0f, 0.0f};
    glm::vec2 max{1.0f, 1.0f};
    
    constexpr UV() = default;
    constexpr UV(float u1, float v1, float u2, float v2) : min(u1, v1), max(u2, v2) {}
    static constexpr UV Full() { return {0, 0, 1, 1}; }
};

// Transform information
struct Transform2D {
    glm::vec2 position{0.0f};
    glm::vec2 scale{1.0f};
    float rotation = 0.0f;
    glm::vec2 origin{0.5f, 0.5f}; // Rotation origin (normalized)
    
    // Helper to build MVP matrix
    glm::mat4 toMatrix() const;
};

// Rendering states and flags
enum class BlendMode : uint8_t {
    None,       // No blending
    Alpha,      // Standard alpha blending
    Additive,   // Additive blending
    Multiply,   // Multiply blending
    Premultiplied // Premultiplied alpha
};

enum class FlipMode : uint8_t {
    None = 0,
    Horizontal = 1,
    Vertical = 2,
    Both = 3
};

// Base command structure with common properties
struct BaseRenderCommand {
    uint32_t sortKey = 0;      // For depth/layer sorting and batching
    Transform2D transform;      // Position, scale, rotation
    Color tint = Color::White();
    BlendMode blendMode = BlendMode::Alpha;
    bool visible = true;
    
protected:
    BaseRenderCommand() = default;
};

// Texture rendering command
struct TextureCommand : BaseRenderCommand {
    TextureHandle texture = 0;
    UV uvRegion = UV::Full();
    FlipMode flipMode = FlipMode::None;
    
    TextureCommand() = default;
    explicit TextureCommand(TextureHandle tex) : texture(tex) {}
};

// Primitive shape commands
struct RectCommand : BaseRenderCommand {
    Rect rect;
    float borderWidth = 0.0f;   // 0 = filled, >0 = outline
    Color borderColor = Color::White();
    
    RectCommand() = default;
    explicit RectCommand(const Rect& r) : rect(r) {}
};

struct CircleCommand : BaseRenderCommand {
    Circle circle;
    float borderWidth = 0.0f;   // 0 = filled, >0 = outline  
    Color borderColor = Color::White();
    uint32_t segments = 32;     // Quality control
    
    CircleCommand() = default;
    explicit CircleCommand(const Circle& c) : circle(c) {}
};

struct LineCommand : BaseRenderCommand {
    Line line;
    
    LineCommand() = default;
    explicit LineCommand(const Line& l) : line(l) {}
};

// Advanced GPU pipeline command
struct CustomCommand : BaseRenderCommand {
    ShaderHandle shader = 0;
    BufferHandle vertexBuffer = 0;
    BufferHandle indexBuffer = 0;
    uint32_t vertexCount = 0;
    uint32_t indexCount = 0;
    
    // Custom uniform data (opaque to the command system)
    std::span<const uint8_t> uniformData;
    
    CustomCommand() = default;
};

// Variant holding all possible render commands
using RenderCommand = std::variant<
    TextureCommand,
    RectCommand, 
    CircleCommand,
    LineCommand,
    CustomCommand
>;

// Camera and viewport
struct Camera2D {
    glm::vec2 position{0.0f};
    glm::vec2 size{1280.0f, 720.0f};
    float zoom = 1.0f;
    float rotation = 0.0f;
    
    glm::mat4 getViewMatrix() const;
    glm::mat4 getProjectionMatrix() const;
    glm::mat4 getViewProjectionMatrix() const;
};

// Render target/viewport
struct Viewport {
    glm::ivec2 position{0, 0};      // x, y
    glm::ivec2 size{1280, 720};     // width, height
    Color clearColor = Color::Black();
    bool clearDepth = true;
    
    // Convenience accessors
    int32_t x() const { return position.x; }
    int32_t y() const { return position.y; }
    int32_t width() const { return size.x; }
    int32_t height() const { return size.y; }
    glm::ivec4 asVec4() const { return {position.x, position.y, size.x, size.y}; }
};

// Complete render frame data
struct RenderFrame {
    Camera2D camera;
    Viewport viewport;
    std::vector<RenderCommand> commands;
    
    // Convenience methods for adding commands
    void addTexture(const TextureCommand& cmd) { commands.emplace_back(cmd); }
    void addRect(const RectCommand& cmd) { commands.emplace_back(cmd); }
    void addCircle(const CircleCommand& cmd) { commands.emplace_back(cmd); }
    void addLine(const LineCommand& cmd) { commands.emplace_back(cmd); }
    void addCustom(const CustomCommand& cmd) { commands.emplace_back(cmd); }
    
    // Clear all commands
    void clear() { commands.clear(); }
    
    // Sort commands by sort key (for batching and depth)
    void sort();
};

// Builder pattern for easier command construction
class TextureCommandBuilder {
public:
    explicit TextureCommandBuilder(TextureHandle tex) { cmd.texture = tex; }
    
    TextureCommandBuilder& position(glm::vec2 pos) { cmd.transform.position = pos; return *this; }
    TextureCommandBuilder& scale(glm::vec2 scale) { cmd.transform.scale = scale; return *this; }
    TextureCommandBuilder& rotation(float rot) { cmd.transform.rotation = rot; return *this; }
    TextureCommandBuilder& tint(Color color) { cmd.tint = color; return *this; }
    TextureCommandBuilder& uv(const UV& uvRegion) { cmd.uvRegion = uvRegion; return *this; }
    TextureCommandBuilder& flip(FlipMode mode) { cmd.flipMode = mode; return *this; }
    TextureCommandBuilder& layer(uint32_t layer) { cmd.sortKey = layer; return *this; }
    TextureCommandBuilder& blend(BlendMode mode) { cmd.blendMode = mode; return *this; }
    
    TextureCommand build() const { return cmd; }
    operator TextureCommand() const { return cmd; }
    
private:
    TextureCommand cmd;
};

class RectCommandBuilder {
public:
    explicit RectCommandBuilder(const Rect& rect) { cmd.rect = rect; }
    
    RectCommandBuilder& position(glm::vec2 pos) { cmd.transform.position = pos; return *this; }
    RectCommandBuilder& scale(glm::vec2 scale) { cmd.transform.scale = scale; return *this; }
    RectCommandBuilder& rotation(float rot) { cmd.transform.rotation = rot; return *this; }
    RectCommandBuilder& color(Color c) { cmd.tint = c; return *this; }
    RectCommandBuilder& border(float width, Color color = Color::White()) { 
        cmd.borderWidth = width; 
        cmd.borderColor = color; 
        return *this; 
    }
    RectCommandBuilder& layer(uint32_t layer) { cmd.sortKey = layer; return *this; }
    RectCommandBuilder& blend(BlendMode mode) { cmd.blendMode = mode; return *this; }
    
    RectCommand build() const { return cmd; }
    operator RectCommand() const { return cmd; }
    
private:
    RectCommand cmd;
};

// Convenience factory functions
inline TextureCommandBuilder Texture(TextureHandle handle) {
    return TextureCommandBuilder(handle);
}

inline RectCommandBuilder Rect(float x, float y, float w, float h) {
    return RectCommandBuilder({x, y, w, h});
}

inline RectCommandBuilder Rect(const glm::vec4& rect) {
    return RectCommandBuilder(rect);
}

inline RectCommandBuilder Rect(const glm::vec2& position, const glm::vec2& size) {
    return RectCommandBuilder({position.x, position.y, size.x, size.y});
}

// Additional GLM convenience functions
namespace RectUtils {
    inline glm::vec2 getPosition(const Rect& rect) { return {rect.x, rect.y}; }
    inline glm::vec2 getSize(const Rect& rect) { return {rect.z, rect.w}; }
    inline glm::vec2 getCenter(const Rect& rect) { return {rect.x + rect.z * 0.5f, rect.y + rect.w * 0.5f}; }
    inline glm::vec2 getTopLeft(const Rect& rect) { return {rect.x, rect.y}; }
    inline glm::vec2 getTopRight(const Rect& rect) { return {rect.x + rect.z, rect.y}; }
    inline glm::vec2 getBottomLeft(const Rect& rect) { return {rect.x, rect.y + rect.w}; }
    inline glm::vec2 getBottomRight(const Rect& rect) { return {rect.x + rect.z, rect.y + rect.w}; }
    
    inline bool contains(const Rect& rect, const glm::vec2& point) {
        return point.x >= rect.x && point.x <= rect.x + rect.z &&
               point.y >= rect.y && point.y <= rect.y + rect.w;
    }
    
    inline bool intersects(const Rect& a, const Rect& b) {
        return !(a.x + a.z < b.x || b.x + b.z < a.x || a.y + a.w < b.y || b.y + b.w < a.y);
    }
}

} // namespace BL::Render