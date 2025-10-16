// Example usage of the new rendering system

#include "Renderer2D.hpp"
#include "RenderCommands.hpp"

using namespace BL::Render;

class GameRenderer {
public:
    explicit GameRenderer(std::unique_ptr<IRenderBackend> backend) 
        : renderer(std::move(backend)) {
        
        // Load some textures
        playerTexture = renderer.loadTexture("assets/player.png");
        backgroundTexture = renderer.loadTexture("assets/background.png");
        
        // Setup camera
        camera.position = {0, 0};
        camera.size = {1280, 720};
        camera.zoom = 1.0f;
    }
    
    void renderGame(float deltaTime) {
        // Begin frame
        renderer.beginFrame();
        renderer.setCamera(camera);
        
        // Method 1: High-level API (simple and clean)
        drawBackground();
        drawPlayer();
        drawUI();
        
        // Method 2: Builder pattern (fluent and powerful)
        drawEnemiesWithBuilder();
        
        // Method 3: Low-level commands (maximum control)
        drawParticlesWithCommands();
        
        // Method 4: Direct frame manipulation (batch operations)
        drawTilemapBatch();
        
        renderer.endFrame();
    }
    
private:
    void drawBackground() {
        // Simple texture rendering
        Transform2D bgTransform;
        bgTransform.position = {640, 360};
        bgTransform.scale = {1.0f, 1.0f};
        
        renderer.drawTexture(backgroundTexture, bgTransform);
    }
    
    void drawPlayer() {
        // Player with tint and rotation
        Transform2D playerTransform;
        playerTransform.position = playerPos;
        playerTransform.rotation = playerRotation;
        playerTransform.scale = {2.0f, 2.0f};
        
        Color playerTint = playerDamaged ? Color::Red() : Color::White();
        
        renderer.drawTexture(playerTexture, playerTransform, playerTint);
    }
    
    void drawUI() {
        // Health bar background (using GLM vec4: x, y, width, height)
        Rect healthBg = {50, 50, 200, 20};
        renderer.drawRect(healthBg, Color::Black());
        
        // Health bar fill
        Rect healthFill = {50, 50, 200 * playerHealth, 20};
        renderer.drawRect(healthFill, Color::Green());
        
        // Health bar border
        renderer.drawRectOutline(healthBg, Color::White(), 2.0f);
        
        // Alternative GLM-style construction
        glm::vec2 uiPos = {50, 100};
        glm::vec2 buttonSize = {120, 40};
        Rect buttonRect = Rect(uiPos, buttonSize);
        renderer.drawRect(buttonRect, Color::Blue());
    }
    
    void drawEnemiesWithBuilder() {
        for (const auto& enemy : enemies) {
            // Builder pattern - very readable and flexible
            auto enemyCmd = Texture(enemyTexture)
                .position(enemy.position)
                .scale({1.5f, 1.5f})
                .rotation(enemy.rotation)
                .tint(enemy.color)
                .layer(100)  // Behind player
                .blend(BlendMode::Alpha);
                
            renderer.submit(enemyCmd);
        }
    }
    
    void drawParticlesWithCommands() {
        // Direct command creation for maximum control
        for (const auto& particle : particles) {
            TextureCommand cmd;
            cmd.texture = particleTexture;
            cmd.transform.position = particle.position;
            cmd.transform.scale = {particle.size, particle.size};
            cmd.transform.rotation = particle.rotation;
            cmd.tint = particle.color;
            cmd.blendMode = BlendMode::Additive;  // Glowy particles
            cmd.sortKey = 1000 + particle.layer;  // Front layer
            
            renderer.submit(cmd);
        }
    }
    
    void drawTilemapBatch() {
        // Direct frame access for batch operations
        auto& frame = renderer.getCurrentFrame();
        
        for (int y = 0; y < mapHeight; ++y) {
            for (int x = 0; x < mapWidth; ++x) {
                if (auto tileId = getTileAt(x, y); tileId != 0) {
                    TextureCommand tileCmd;
                    tileCmd.texture = tilesetTexture;
                    tileCmd.transform.position = {x * tileSize, y * tileSize};
                    tileCmd.uvRegion = getTileUV(tileId);  // Atlas coordinates
                    tileCmd.sortKey = 0;  // Background layer
                    
                    frame.addTexture(tileCmd);
                }
            }
        }
    }
    
    void drawShapes() {
        // Drawing primitive shapes
        Circle circle = {{400, 300}, 50};
        renderer.drawCircle(circle, Color::Blue());
        
        Line line = {{100, 100}, {200, 200}, 3.0f};
        renderer.drawLine(line, Color::Red());
        
        // Custom shapes with GPU pipeline
        if (customShader != 0) {
            CustomCommand customCmd;
            customCmd.shader = customShader;
            customCmd.vertexBuffer = customMeshBuffer;
            customCmd.vertexCount = customMeshVertexCount;
            customCmd.transform.position = {500, 400};
            customCmd.sortKey = 500;
            
            renderer.submit(customCmd);
        }
    }
    
private:
    Renderer2D renderer;
    Camera2D camera;
    
    // Game state
    glm::vec2 playerPos{640, 360};
    float playerRotation = 0.0f;
    float playerHealth = 0.8f;  // 80%
    bool playerDamaged = false;
    
    struct Enemy {
        glm::vec2 position;
        float rotation;
        Color color = Color::White();
    };
    std::vector<Enemy> enemies;
    
    struct Particle {
        glm::vec2 position;
        float size;
        float rotation;
        Color color;
        int layer;
    };
    std::vector<Particle> particles;
    
    // Textures
    TextureHandle playerTexture = 0;
    TextureHandle enemyTexture = 0;
    TextureHandle backgroundTexture = 0;
    TextureHandle particleTexture = 0;
    TextureHandle tilesetTexture = 0;
    
    // Custom GPU resources
    ShaderHandle customShader = 0;
    BufferHandle customMeshBuffer = 0;
    uint32_t customMeshVertexCount = 0;
    
    // Tilemap
    int mapWidth = 40, mapHeight = 30, tileSize = 32;
    
    int getTileAt(int x, int y) const {
        // Dummy implementation
        return (x + y) % 4;  // Some pattern
    }
    
    UV getTileUV(int tileId) const {
        // Convert tile ID to UV coordinates in atlas
        float tileU = (tileId % 8) * 0.125f;  // 8 tiles per row
        float tileV = (tileId / 8) * 0.125f;  // 8 tiles per column
        return UV(tileU, tileV, tileU + 0.125f, tileV + 0.125f);
    }
};

// Usage in main game loop
void gameLoop() {
    // Create SDL3 backend (or any other backend)
    auto sdlBackend = std::make_unique<SDL3RenderBackend>();
    
    GameRenderer gameRenderer(std::move(sdlBackend));
    
    // Game loop
    while (running) {
        float deltaTime = calculateDeltaTime();
        
        // Update game logic
        updateGame(deltaTime);
        
        // Render
        gameRenderer.renderGame(deltaTime);
    }
}