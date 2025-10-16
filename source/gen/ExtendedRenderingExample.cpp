// Example showing how to use your excellent existing system with extensions

#include "RenderCommandExtensions.hpp"
#include "Engine.hpp"

class MyGameModule : public BaseModule {
public:
    void onInit() override {
        // Load textures using your engine
        playerTexture = engine->loadBMPTexture("assets/player.bmp");
        backgroundTexture = engine->loadBMPTexture("assets/background.bmp");
        
        // Create extended renderer
        extRenderer = std::make_unique<BL::ExtendedRenderer2D>();
    }
    
    void extract(RenderSnapshots2D& snapshots) override {
        // Method 1: Use your existing QuadCmd system directly (best performance)
        renderWithQuadCmds(snapshots);
        
        // Method 2: Use extended renderer for convenience
        renderWithExtendedAPI(snapshots);
        
        // Merge results
        auto& extSnapshots = extRenderer->getSnapshots();
        snapshots.insert(snapshots.end(), extSnapshots.begin(), extSnapshots.end());
    }
    
private:
    void renderWithQuadCmds(RenderSnapshots2D& snapshots) {
        // Your existing approach - keep this for performance-critical rendering!
        OrthoCamera camera;
        camera.view = glm::mat4(1.0f);
        camera.proj = glm::ortho(0.0f, 1280.0f, 720.0f, 0.0f);
        
        RenderSnapshot2D snapshot;
        snapshot.cam = camera;
        
        // Direct QuadCmd creation (your current style)
        QuadCmd playerQuad;
        playerQuad.tex = playerTexture;
        playerQuad.mvp = camera.proj * camera.view * getPlayerTransform();
        playerQuad.color = playerDamaged ? glm::vec4(1,0,0,1) : glm::vec4(1,1,1,1);
        playerQuad.uv = {0, 0, 1, 1};
        playerQuad.sortKey = 100;
        
        snapshot.quads.push_back(playerQuad);
        
        // Using builder pattern with your QuadCmd
        auto backgroundQuad = BL::Quad()
            .texture(backgroundTexture)
            .mvp(camera.proj * camera.view * getBackgroundTransform())
            .color({1, 1, 1, 1})
            .uv(0, 0, 1, 1)
            .sortKey(0);
        
        snapshot.quads.push_back(backgroundQuad);
        
        snapshots.push_back(std::move(snapshot));
    }
    
    void renderWithExtendedAPI(RenderSnapshots2D& snapshots) {
        // High-level API for UI and simple shapes
        OrthoCamera camera;
        camera.view = glm::mat4(1.0f); 
        camera.proj = glm::ortho(0.0f, 1280.0f, 720.0f, 0.0f);
        
        extRenderer->beginSnapshot(camera);
        
        // Draw shapes easily
        extRenderer->drawRect({50, 50, 200, 20}, {0, 0, 0, 1}, 50);     // Health bar bg
        extRenderer->drawRect({50, 50, 200 * playerHealth, 20}, {0, 1, 0, 1}, 51); // Health fill
        
        // Draw UI elements
        extRenderer->drawCircle({100, 100}, 25, {1, 0, 0, 1}, 60);     // Red circle
        extRenderer->drawLine({200, 200}, {300, 300}, 3.0f, {1, 1, 0, 1}, 70); // Yellow line
        
        extRenderer->endSnapshot();
    }
    
    glm::mat4 getPlayerTransform() const {
        glm::mat4 transform(1.0f);
        transform = glm::translate(transform, glm::vec3(playerPos, 0.0f));
        transform = glm::rotate(transform, playerRotation, glm::vec3(0, 0, 1));
        transform = glm::scale(transform, glm::vec3(playerScale, 1.0f));
        return transform;
    }
    
    glm::mat4 getBackgroundTransform() const {
        return glm::scale(glm::mat4(1.0f), glm::vec3(1280, 720, 1));
    }
    
private:
    std::unique_ptr<BL::ExtendedRenderer2D> extRenderer;
    TextureHandle playerTexture = 0;
    TextureHandle backgroundTexture = 0;
    
    glm::vec2 playerPos{640, 360};
    float playerRotation = 0.0f;
    glm::vec2 playerScale{1.0f, 1.0f};
    float playerHealth = 0.8f;
    bool playerDamaged = false;
};

// Your engine integration remains unchanged!
// The BaseModule system, Engine, EventStack - all perfect as-is