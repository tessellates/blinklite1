#include <iostream>

#include "source/application/sdl_base_app.hpp"
#include "source/application/Engine.hpp"
#include "source/ConnectGame.hpp"
#include <memory>

int main() {
    // Create engine with config
    EngineConfig config;
    config.windowTitle = "Connect 4 - BlinkLite Engine";
    config.windowWidth = 800;
    config.windowHeight = 600;
    config.fullscreen = false;
    
    // Create and run the application
    return BlinkLiteApp::run(config, [](Engine& engine) {
        // Create and register the Connect game module
        auto connectGame = std::make_unique<ConnectGame>(engine);
        engine.addModule(std::move(connectGame));
        
        // Game callbacks
        GameCallbacks callbacks;
        callbacks.onGameInit = []() {
            std::cout << "Connect 4 Game Starting!" << std::endl;
        };
        
        callbacks.onGameQuit = []() {
            std::cout << "Connect 4 Game Ending!" << std::endl;
        };
        
        engine.setCallbacks(callbacks);
    });
}
