// Example showing the complete integrated system

#include "Engine_Fixed.hpp"  // or Engine_Clean.hpp
#include "ShapeRenderer.hpp"
#include "sdl_base_app.hpp"
#include <glm/gtc/matrix_transform.hpp>

class ExampleGameModule : public BaseModule {
public:
    void onInit() override {
        // Load textures using your engine's texture system
        playerTexture = engine->loadBMPTexture("assets/player.bmp");
        backgroundTexture = engine->loadBMPTexture("assets/background.bmp");
        
        // Initialize shape renderer
        shapeRenderer = std::make_unique<ShapeRenderer>(*engine->getTextureManager());
        shapeRenderer->init(); // Creates white pixel texture
    }
    
    void tick(float dt, std::span<const EventData> events) override {
        // Update game logic
        playerRotation += dt * 45.0f; // Rotate 45 degrees per second
        
        // Handle input
        for (const auto& event : events) {
            if (event.action == Event::MoveX) {
                const F1* data = static_cast<const F1*>(event.data);
                playerPos.x += data->v * dt * 300.0f; // Move 300 pixels/sec
            }
            if (event.action == Event::MoveY) {
                const F1* data = static_cast<const F1*>(event.data);
                playerPos.y += data->v * dt * 300.0f;
            }
        }
    }
    
    void extract(RenderSnapshots2D& snapshots) override {
        // Create camera
        OrthoCamera camera;
        camera.view = glm::mat4(1.0f);
        camera.proj = glm::ortho(0.0f, 1280.0f, 720.0f, 0.0f);
        
        RenderSnapshot2D snapshot;
        snapshot.cam = camera;
        
        // Method 1: Direct QuadCmd creation (your existing style - best performance)
        renderWithQuadCmds(snapshot, camera);
        
        // Method 2: Use shape renderer for UI and primitives  
        renderWithShapes(snapshot);
        
        snapshots.push_back(std::move(snapshot));
    }
    
private:
    void renderWithQuadCmds(RenderSnapshot2D& snapshot, const OrthoCamera& camera) {
        // Background quad (your existing QuadCmd style)
        QuadCmd backgroundQuad;
        backgroundQuad.tex = backgroundTexture;
        backgroundQuad.mvp = camera.proj * camera.view * getBackgroundTransform();
        backgroundQuad.color = {1, 1, 1, 1};
        backgroundQuad.uv = {0, 0, 1, 1};
        backgroundQuad.sortKey = 0; // Background layer
        snapshot.quads.push_back(backgroundQuad);
        
        // Player quad with builder pattern
        auto playerQuad = Quad()
            .texture(playerTexture)
            .mvp(camera.proj * camera.view * getPlayerTransform())
            .color(playerDamaged ? glm::vec4{1, 0, 0, 1} : glm::vec4{1, 1, 1, 1})
            .uv(0, 0, 1, 1)
            .sortKey(100); // Player layer
            
        snapshot.quads.push_back(playerQuad);
    }
    
    void renderWithShapes(RenderSnapshot2D& snapshot) {
        // Health bar using shape renderer
        Rect healthBarBg = {50, 50, 200, 20}; // GLM vec4: x, y, width, height
        shapeRenderer->addRect(snapshot, healthBarBg, {0, 0, 0, 1}, 200); // UI layer
        
        Rect healthBarFill = {50, 50, 200 * playerHealth, 20};
        shapeRenderer->addRect(snapshot, healthBarFill, {0, 1, 0, 1}, 201); // Above background
        
        shapeRenderer->addRectOutline(snapshot, healthBarBg, {1, 1, 1, 1}, 2.0f, 202);
        
        // Mini-map using shapes
        Circle miniMapCircle = {{1150, 100}, 50};
        shapeRenderer->addCircle(snapshot, miniMapCircle, {0, 0, 0.5f, 0.7f}, 190, 16); // Semi-transparent
        
        // Draw line to player position on minimap
        glm::vec2 miniMapCenter = {1150, 100};
        glm::vec2 playerOnMap = miniMapCenter + (playerPos / glm::vec2{1280, 720}) * 40.0f;
        
        Line playerLine = {miniMapCenter, playerOnMap, 2.0f};
        shapeRenderer->addLine(snapshot, playerLine, {1, 1, 0, 1}, 191);
    }
    
    glm::mat4 getPlayerTransform() const {
        glm::mat4 transform(1.0f);
        transform = glm::translate(transform, glm::vec3(playerPos, 0.0f));
        transform = glm::rotate(transform, glm::radians(playerRotation), glm::vec3(0, 0, 1));
        transform = glm::scale(transform, glm::vec3(64, 64, 1)); // 64x64 sprite
        return transform;
    }
    
    glm::mat4 getBackgroundTransform() const {
        return glm::scale(glm::mat4(1.0f), glm::vec3(1280, 720, 1));
    }
    
private:
    std::unique_ptr<ShapeRenderer> shapeRenderer;
    
    // Game state
    glm::vec2 playerPos{640, 360};
    float playerRotation = 0.0f;
    float playerHealth = 0.75f; // 75%
    bool playerDamaged = false;
    
    // Textures
    TextureHandle playerTexture = 0;
    TextureHandle backgroundTexture = 0;
};

// Example application using your existing system
class ExampleApp : public EngineAppBase {
public:
    bool onInit() override {
        // Initialize your engine (same as before)
        EngineConfig config;
        config.title = "Combined Rendering Example";
        config.w = 1280;
        config.h = 720;
        config.vsync = true;
        
        if (!eng.init(config)) {
            return false;
        }
        
        // Add our example module
        gameModule = std::make_unique<ExampleGameModule>();
        addModule(gameModule.get());
        
        return true;
    }
    
private:
    std::unique_ptr<ExampleGameModule> gameModule;
};

// Entry point using your existing macro system
ENGINE_DEFINE_SDL_APP(ExampleApp)