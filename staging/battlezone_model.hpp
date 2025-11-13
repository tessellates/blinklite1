#pragma once
#include <CoreTypes.hpp>
#include <Event.hpp>
#include <vector>
#include <cmath>
#include <random>
#include <algorithm>

struct Tank {
    float x = 0, z = 0;
    float angle = 0;
    float speed = 0;
    bool alive = true;
};

struct Enemy {
    float x, z;
    float angle;
    bool alive = true;
};

struct Shot {
    float x, z;
    float dx, dz;
    float life = 3.0f;
};

struct BattleZoneModel {
    Tank player;
    std::vector<Enemy> enemies;
    std::vector<Shot> shots;
    float spawnTimer = 0;
    int score = 0;
};

struct BattleZoneContext {
    float screenWidth;
    float screenHeight;
    float horizon;
};

inline void battlezone_input(BattleZoneModel& m, const EventData& e) {
    if (e.action == Event::KeyDown) {
        KeyboardKey key = *static_cast<const KeyboardKey*>(e.data);
        switch (key) {
            case KEY_W:
            case KEY_UP:
                m.player.speed = 10.0f;
                break;
                
            case KEY_S:
            case KEY_DOWN:
                m.player.speed = -5.0f;
                break;
                
            case KEY_A:
            case KEY_LEFT:
                m.player.angle -= 0.1f;
                break;
                
            case KEY_D:
            case KEY_RIGHT:
                m.player.angle += 0.1f;
                break;
                
            case KEY_SPACE:
                // Fire shot
                m.shots.push_back({
                    m.player.x, m.player.z,
                    sinf(m.player.angle) * 200.0f,
                    cosf(m.player.angle) * 200.0f,
                    3.0f
                });
                break;
                
            default:
                break;
        }
    } else if (e.action == Event::KeyUp) {
        KeyboardKey key = *static_cast<const KeyboardKey*>(e.data);
        switch (key) {
            case KEY_W:
            case KEY_UP:
            case KEY_S:
            case KEY_DOWN:
                m.player.speed = 0;
                break;
                
            default:
                break;
        }
    }
    std::cout << "speed:" << m.player.speed << std::endl;
}

inline void battlezone_tick(BattleZoneModel& m, float dt) {
    // Move player
    m.player.x += sinf(m.player.angle) * m.player.speed * dt;
    m.player.z += cosf(m.player.angle) * m.player.speed * dt;
        // Move shots   
    for (auto& shot : m.shots) {
        shot.x += shot.dx * dt;
        shot.z += shot.dz * dt;
        shot.life -= dt;
    }
    
    // Remove dead shots
    m.shots.erase(
        std::remove_if(m.shots.begin(), m.shots.end(), 
                      [](const Shot& s) { return s.life <= 0; }),
        m.shots.end());
    
    // Spawn enemies
    m.spawnTimer += dt;
    if (m.spawnTimer > 2.0f && m.enemies.size() < 5) {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        std::uniform_real_distribution<> dis(-500, 500);
        
        m.enemies.push_back({
            (float)dis(gen), (float)dis(gen), 0, true
        });
        m.spawnTimer = 0;
    }
    
    // Check collisions (shots vs enemies)
    for (auto& shot : m.shots) {
        for (auto& enemy : m.enemies) {
            if (enemy.alive) {
                float dx = shot.x - enemy.x;
                float dz = shot.z - enemy.z;
                if (dx*dx + dz*dz < 25*25) {
                    enemy.alive = false;
                    shot.life = 0;
                    m.score += 100;
                }
            }
        }
    }
    
    // Remove dead enemies
    m.enemies.erase(
        std::remove_if(m.enemies.begin(), m.enemies.end(),
                      [](const Enemy& e) { return !e.alive; }),
        m.enemies.end());
}

inline void battlezone_extract(const BattleZoneModel& m, BattleZoneContext context, RenderSnapshot2D& out) {
    out.quads.clear();
    
    // Draw horizon line
    QuadCmd horizon;
    horizon.position = {0, context.horizon};
    horizon.size = {context.screenWidth, 2};
    horizon.color = {0, 1, 0, 1}; // Green
    horizon.tex = 0;
    out.quads.push_back(horizon);
    
    // Simple 3D-to-2D projection function
    auto project = [&](float worldX, float worldZ) -> glm::vec2 {
        // Translate to player space
        float relX = worldX - m.player.x;
        float relZ = worldZ - m.player.z;
        
        // Rotate by player angle
        float cos_a = cosf(-m.player.angle);
        float sin_a = sinf(-m.player.angle);
        float rotX = relX * cos_a - relZ * sin_a;
        float rotZ = relX * sin_a + relZ * cos_a;
        
        if (rotZ <= 1.0f) return {-1000, -1000}; // Behind player
        
        // Project to screen
        float screenX = context.screenWidth * 0.5f + (rotX / rotZ) * 400.0f;
        float screenY = context.horizon - (50.0f / rotZ) * 100.0f; // Simple height
        
        return {screenX, screenY};
    };
    
    // Draw enemies as wireframe rectangles
    for (const auto& enemy : m.enemies) {
        if (!enemy.alive) continue;
        
        glm::vec2 pos = project(enemy.x, enemy.z);
        if (pos.x < -500 || pos.x > context.screenWidth + 500) continue;
        
        float size = 1000.0f / (abs(enemy.z - m.player.z) + 1.0f);
        size = std::max(2.0f, std::min(size, 50.0f));
        
        // Enemy body (red rectangle)
        QuadCmd enemyQuad;
        enemyQuad.position = {pos.x - size/2, pos.y - size};
        enemyQuad.size = {size, size};
        enemyQuad.color = {1, 0, 0, 1}; // Red
        enemyQuad.tex = 0;
        out.quads.push_back(enemyQuad);
    }
    
    // Draw shots as small green squares
    for (const auto& shot : m.shots) {
        glm::vec2 pos = project(shot.x, shot.z);
        if (pos.x < 0 || pos.x > context.screenWidth) continue;
        
        QuadCmd shotQuad;
        shotQuad.position = {pos.x - 2, pos.y - 2};
        shotQuad.size = {4, 4};
        shotQuad.color = {0, 1, 1, 1}; // Cyan
        shotQuad.tex = 0;
        out.quads.push_back(shotQuad);
    }
    
    // Draw crosshair
    QuadCmd crosshairH;
    crosshairH.position = {context.screenWidth/2 - 10, context.screenHeight/2 - 1};
    crosshairH.size = {20, 2};
    crosshairH.color = {1, 1, 1, 1}; // White
    crosshairH.tex = 0;
    out.quads.push_back(crosshairH);
    
    QuadCmd crosshairV;
    crosshairV.position = {context.screenWidth/2 - 1, context.screenHeight/2 - 10};
    crosshairV.size = {2, 20};
    crosshairV.color = {1, 1, 1, 1}; // White
    crosshairV.tex = 0;
    out.quads.push_back(crosshairV);
}