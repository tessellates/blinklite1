#pragma once
#include <CoreTypes.hpp>
#include <Event.hpp>
#include <vector>
#include <cmath>
#include <random>
#include <algorithm>
#include <iostream>

struct Tank {
    float x = 0, z = 0;
    float angle = 0;
    float speed = 0;
    float turnSpeed = 0;
    bool alive = true;
    float pitch = 0.0f;      // in radians, looking up/down
    float pitchSpeed = 0.0f;
};

struct Enemy {
    float x, z;
    float angle;
    bool alive = true;
    float spawnTime = 0;
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
    float gameTime = 0;
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
                m.player.speed = 0.1f; // Faster movement
                break;
                
            case KEY_S:
            case KEY_DOWN:
                m.player.speed = -0.05f; // Reverse
                break;
                
            case KEY_A:
            case KEY_LEFT:
                m.player.turnSpeed = -0.0001f; // Smooth turning
                break;
                
            case KEY_D:
            case KEY_RIGHT:
                m.player.turnSpeed = .0001f; // Smooth turning
                break;
                
            case KEY_SPACE:
                // Fire shot with cooldown (prevent spam)
                static float lastShot = 0;
                if (m.gameTime - lastShot > 0.2f) {
                    m.shots.push_back({
                        m.player.x, m.player.z,
                        sinf(m.player.angle) * 300.0f, // Faster shots
                        cosf(m.player.angle) * 300.0f,
                        3.0f
                    });
                    lastShot = m.gameTime;
                }
                break;
            case KEY_Q: m.player.pitchSpeed = -0.001f; break; // look up
            case KEY_E: m.player.pitchSpeed =  0.001f; break; // look down
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
                
            case KEY_A:
            case KEY_LEFT:
            case KEY_D:
            case KEY_RIGHT:
                m.player.turnSpeed = 0;
                break;
            case KEY_Q:
            case KEY_E:
                m.player.pitchSpeed = 0;
                break;
            default:
                break;
        }
    }
}

inline void battlezone_tick(BattleZoneModel& m, float dt) {
    m.gameTime += dt;
    
    m.player.angle += m.player.turnSpeed * dt;

    // Forward = +Z, angle 0 looks down +Z
    m.player.x += sinf(m.player.angle) * m.player.speed * dt;
    m.player.z += cosf(m.player.angle) * m.player.speed * dt;
    m.player.pitch += m.player.pitchSpeed * dt;

    // clamp to avoid looking backwards
    m.player.pitch = std::clamp(m.player.pitch, -0.5f, 0.5f); 
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
    
    // Spawn enemies more strategically
    m.spawnTimer += dt;
    if (m.spawnTimer > 3.0f && m.enemies.size() < 3) {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        
        // Spawn enemies further away in a circle around player
        std::uniform_real_distribution<> angleDis(0, 2 * M_PI);
        std::uniform_real_distribution<> distDis(200, 400);
        
        float spawnAngle = (float)angleDis(gen);
        float spawnDist = (float)distDis(gen);
        
        m.enemies.push_back({
            m.player.x + cosf(spawnAngle) * spawnDist,
            m.player.z + sinf(spawnAngle) * spawnDist,
            0, true, m.gameTime
        });
        m.spawnTimer = 0;
    }
    
    // Move enemies toward player (simple AI)
    /*
    for (auto& enemy : m.enemies) {
        if (!enemy.alive) continue;
        
        float dx = m.player.x - enemy.x;
        float dz = m.player.z - enemy.z;
        float dist = sqrtf(dx*dx + dz*dz);
        
        if (dist > 5.0f) {
            enemy.x += (dx / dist) * 30.0f * dt; // Move toward player
            enemy.z += (dz / dist) * 30.0f * dt;
        }
    }*/
    
    // Check collisions (shots vs enemies)
    for (auto& shot : m.shots) {
        for (auto& enemy : m.enemies) {
            if (enemy.alive) {
                float dx = shot.x - enemy.x;
                float dz = shot.z - enemy.z;
                if (dx*dx + dz*dz < 30*30) { // Larger hit radius
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
    
    // Draw ground (dark green below horizon)
    QuadCmd ground;
    ground.position = {0, context.horizon};
    ground.size = {context.screenWidth, context.screenHeight - context.horizon};
    ground.color = {0, 0.2f, 0, 1}; // Dark green ground
    ground.tex = 0;
    out.quads.push_back(ground);
    
    // Draw sky (dark blue above horizon)
    QuadCmd sky;
    sky.position = {0, 0};
    sky.size = {context.screenWidth, context.horizon};
    sky.color = {0, 0, 0.2f, 1}; // Dark blue sky
    sky.tex = 0;
    out.quads.push_back(sky);
    
    // Draw horizon line (brighter green)
    QuadCmd horizon;
    horizon.position = {0, context.horizon - 1};
    horizon.size = {context.screenWidth, 2};
    horizon.color = {0, 0.8f, 0, 1}; // Bright green horizon
    horizon.tex = 0;
    out.quads.push_back(horizon);
    
    auto project = [&](float worldX, float worldZ) -> glm::vec2 {
        float relX = worldX - m.player.x;
        float relZ = worldZ - m.player.z;

        float ca = cosf(m.player.angle);
        float sa = sinf(m.player.angle);

        float camX =  relX * ca - relZ * sa;
        float camZ =  relX * sa + relZ * ca;

        if (camZ <= 1.0f)
            return {-10000, -10000};

        float perspective = 800.0f / camZ;

        float screenX = context.screenWidth * 0.5f + camX * perspective;

        float objectHeight = 20.0f;

        // pitch controls vertical look direction
        float pitchOffset = tanf(m.player.pitch) * camZ;

        float screenY = context.horizon 
            + (objectHeight * perspective)
            - pitchOffset;

        return {screenX, screenY};
    };
        
    // Draw enemies with better visuals
    for (const auto& enemy : m.enemies) {
        if (!enemy.alive) continue;
        
        glm::vec2 pos = project(enemy.x, enemy.z);
        if (pos.x < -200 || pos.x > context.screenWidth + 200) continue;
        
        float distance = sqrtf(powf(enemy.x - m.player.x, 2) + powf(enemy.z - m.player.z, 2));
        float size = 1200.0f / (distance + 10.0f);
        size = std::max(3.0f, std::min(size, 60.0f));
        
        // Enemy tank body (red)
        QuadCmd enemyBody;
        enemyBody.position = {pos.x - size/2, pos.y - size};
        enemyBody.size = {size, size * 0.6f};
        enemyBody.color = {1, 0.2f, 0.2f, 1}; // Bright red
        enemyBody.tex = 0;
        out.quads.push_back(enemyBody);
        
        // Enemy tank turret (darker red)
        QuadCmd enemyTurret;
        enemyTurret.position = {pos.x - size/4, pos.y - size*0.8f};
        enemyTurret.size = {size/2, size/3};
        enemyTurret.color = {0.8f, 0, 0, 1}; // Dark red
        enemyTurret.tex = 0;
        out.quads.push_back(enemyTurret);
    }
    
    // Draw shots (brighter and bigger)
    for (const auto& shot : m.shots) {
        glm::vec2 pos = project(shot.x, shot.z);
        if (pos.x < -50 || pos.x > context.screenWidth + 50) continue;
        
        QuadCmd shotQuad;
        shotQuad.position = {pos.x - 3, pos.y - 3};
        shotQuad.size = {6, 6};
        shotQuad.color = {1, 1, 0, 1}; // Bright yellow shots
        shotQuad.tex = 0;
        out.quads.push_back(shotQuad);
    }
    
    // Draw crosshair (better visibility)
    QuadCmd crosshairH;
    crosshairH.position = {context.screenWidth/2 - 15, context.screenHeight/2 - 1};
    crosshairH.size = {30, 2};
    crosshairH.color = {1, 1, 1, 0.8f}; // Semi-transparent white
    crosshairH.tex = 0;
    out.quads.push_back(crosshairH);
    
    QuadCmd crosshairV;
    crosshairV.position = {context.screenWidth/2 - 1, context.screenHeight/2 - 15};
    crosshairV.size = {2, 30};
    crosshairV.color = {1, 1, 1, 0.8f}; // Semi-transparent white
    crosshairV.tex = 0;
    out.quads.push_back(crosshairV);
    
    // Draw score (simple digits using quads - top right)
    // This is basic, but shows the score as white rectangles
    if (m.score > 0) {
        QuadCmd scoreIndicator;
        scoreIndicator.position = {context.screenWidth - 100, 20};
        scoreIndicator.size = {80, 20};
        scoreIndicator.color = {0.3f, 0.3f, 0.3f, 0.7f}; // Dark background for score
        scoreIndicator.tex = 0;
        out.quads.push_back(scoreIndicator);
    }
}