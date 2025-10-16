# Rendering System Design Comparison

## Current System Issues
- Tightly coupled to SDL3 types (`SDL_FlipMode`, `SDL_FRect`, etc.)
- Limited to texture rendering only
- No support for primitive shapes or custom GPU pipelines
- Manual layer management
- No command batching or sorting
- Mixed concerns (rendering logic with SDL specifics)

## New System Benefits

### 1. **Complete SDL3 Abstraction**
```cpp
// Old: SDL3-dependent
struct RenderInfo {
    SDL_FRect clip;
    SDL_FlipMode flip;
    // ... SDL3 types everywhere
};

// New: Platform-agnostic
struct TextureCommand : BaseRenderCommand {
    UV uvRegion = UV::Full();
    FlipMode flipMode = FlipMode::None;
    // ... No SDL3 dependencies
};
```

### 2. **Flexible Rendering Types with GLM Integration**
```cpp
// Old: Only textures
void renderTexture(TextureHandle tex, Transform transform);

// New: Multiple render types using GLM
renderer.drawTexture(texture, transform);
renderer.drawRect(glm::vec4{100, 100, 50, 30}, Color::Red());  // x,y,w,h
renderer.drawRect(position, size, Color::Blue());              // separate pos/size
renderer.drawCircle(circle, Color::Green());
renderer.drawLine(line, Color::Yellow());
renderer.submit(customGPUCommand);  // Custom shaders!

// GLM convenience utilities
glm::vec2 center = RectUtils::getCenter(rect);
bool hit = RectUtils::contains(buttonRect, mousePos);
```

### 3. **Clean API with Multiple Usage Patterns**

#### Simple API with GLM:
```cpp
renderer.drawTexture(playerTexture, playerTransform, Color::White());

// Rectangle rendering with GLM vectors
glm::vec2 pos = {100, 200};
glm::vec2 size = {64, 32};
renderer.drawRect(pos, size, Color::Blue());

// Or as vec4 (x, y, width, height)
renderer.drawRect(glm::vec4{100, 200, 64, 32}, Color::Red());
```

#### Builder Pattern:
```cpp
auto cmd = Texture(enemyTexture)
    .position({100, 200})
    .scale({2.0f, 2.0f})
    .rotation(45.0f)
    .tint(Color::Red())
    .layer(100);
renderer.submit(cmd);
```

#### Direct Commands:
```cpp
TextureCommand cmd;
cmd.texture = tex;
cmd.transform.position = pos;
cmd.sortKey = layer;
renderer.submit(cmd);
```

### 4. **Automatic Batching and Sorting**
```cpp
// Commands are automatically sorted by sortKey for:
// - Depth/layer ordering
// - Batching same textures/materials
// - Optimizing draw calls

frame.sort();  // Automatic optimization
```

### 5. **Backend Flexibility**
```cpp
// Can swap rendering backends easily:
auto sdlBackend = std::make_unique<SDL3Backend>();
auto openglBackend = std::make_unique<OpenGLBackend>();
auto vulkanBackend = std::make_unique<VulkanBackend>();

Renderer2D renderer(std::move(sdlBackend));
```

## Migration Path

### Phase 1: Core Structure
1. Implement `RenderCommands.hpp` (done)
2. Create `IRenderBackend` interface (done)
3. Implement SDL3 backend inheriting from `IRenderBackend`

### Phase 2: Basic Commands
1. Implement texture rendering in SDL3 backend
2. Add rectangle and circle primitives
3. Test with existing game code

### Phase 3: Advanced Features
1. Add custom shader support
2. Implement command batching
3. Add advanced blending modes

### Phase 4: Optimization
1. Add instanced rendering for repeated objects
2. Implement texture atlasing
3. Add GPU-side culling

## Key Design Principles

### **Minimalistic**
- Each command is a simple data structure
- No virtual functions in commands (performance)
- Clear separation of concerns

### **Flexible** 
- Support for textures, shapes, and custom GPU work
- Multiple API styles (simple, builder, direct)
- Easy to extend with new command types

### **Backend Agnostic**
- No SDL3/OpenGL/Vulkan types in public API
- Easy to test with mock backends
- Can support multiple backends simultaneously

### **Performance Oriented**
- Commands are lightweight structs
- Automatic batching and sorting
- GPU-friendly data layout