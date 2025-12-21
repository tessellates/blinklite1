#pragma once
#include <CoreTypes.hpp>
#include <Event.hpp>
#include <vector>
#include <cmath>
#include <random>
#include <algorithm>

struct SandBlock {
    float x, z;       // world position
    float size;       // width on ground
    float speed;      // forward speed
    glm::vec4 color;  // block color
};

struct Tank {
    float x = 0;
    float z = 0;
    float speed = 0;
    bool alive = true;
};

struct SandSlideModel {
    Tank player;
    std::vector<SandBlock> blocks;
    float spawnTimer = 0;
    float gameTime = 0;
    int score = 0;
    float mod = 1000;
};

struct BattleZoneContext {
    float screenWidth;
    float screenHeight;
    float horizon; // y of horizon
};

//
// INPUT
//
inline void sandslide_input(SandSlideModel& m, const EventData& e)
{
    
    if (e.action==Event::MoveX)
    {
        float y = static_cast<const F1*>(e.data)->v;
        if (y>0.5f)  m.player.speed = 200.0f;
        else
        if (y<-0.5f) m.player.speed = -200.0f;
    } 
    else m.player.speed = 0;

    if (e.action == Event::KeyDown) {
        KeyboardKey key = *static_cast<const KeyboardKey*>(e.data);
        std::cout << "Key down: " << static_cast<int>(key) << "\n";
        if (key == KEY_LSHIFT || key == KEY_RSHIFT)
        {
            m.mod = m.mod/2;
        }
    }

    if (e.action == Event::KeyUp) {
        KeyboardKey key = *static_cast<const KeyboardKey*>(e.data);
        std::cout << "Key up: " << static_cast<int>(key) << "\n";
        if (key == KEY_LSHIFT || key == KEY_RSHIFT)
        {
            m.mod = m.mod*2;
        }
    }
}

//
// TICK
//
inline void sandslide_tick(SandSlideModel& m, float dt)
{
    dt = dt/m.mod;
    m.gameTime += dt;

    // Move player sideways
    m.player.x += m.player.speed * dt;

    // Spawn blocks
    m.spawnTimer += dt;
    if (m.spawnTimer > 0.6f) {
        m.spawnTimer = 0;

        SandBlock b;
        b.x = (rand() % 600 - 300);   // random between -300..300
        b.z = 1200;                   // far away
        b.size = 40 + (rand() % 80);
        b.speed = 200 + (rand() % 200);
        b.color = {1, 0.8f, 0, 1};

        m.blocks.push_back(b);
    }

    // Move blocks toward the player
    for (auto& b : m.blocks) {
        b.z -= b.speed * dt;
    }

    // Collision + scoring
    for (auto& b : m.blocks) {
        if (b.z < 50) {
            if (fabs(b.x - m.player.x) < b.size * 0.5f)
                m.player.alive = false;
            else
                m.score += 1;
        }
    }

    // Remove passed blocks
    m.blocks.erase(
        std::remove_if(m.blocks.begin(), m.blocks.end(),
            [](const SandBlock& b) { return b.z < -100; }),
        m.blocks.end());
}

//
// RENDER
//
inline void sandslide_extract(
    const SandSlideModel& m,
    BattleZoneContext ctx,
    RenderSnapshot2D& out)
{
    out.quads.clear();

    //
    // Draw sky
    //
    {
        QuadCmd sky;
        sky.position = {0, 0};
        sky.size = {ctx.screenWidth, ctx.horizon};
        sky.color = {0.05f, 0.05f, 0.2f, 1};
        sky.tex = 0;
        out.quads.push_back(sky);
    }

    //
    // Draw ground
    //
    {
        QuadCmd ground;
        ground.position = {0, ctx.horizon};
        ground.size = {ctx.screenWidth, ctx.screenHeight - ctx.horizon};
        ground.color = {0.4f, 0.3f, 0.1f, 1};
        ground.tex = 0;
        out.quads.push_back(ground);
    }

    //
    // Projection: simple pseudo-3D
    //
    auto project = [&](float worldX, float worldZ) -> glm::vec2 {
        float relX = worldX - m.player.x;
        float relZ = worldZ - m.player.z;   // player.z = 0, so just worldZ

        if (relZ <= 1.0f)
            return {-10000, -10000};

        float perspective = 900.0f / relZ;

        float screenX = ctx.screenWidth * 0.5f + relX * perspective;
        float screenY = ctx.horizon + (20.0f * perspective);

        return {screenX, screenY};
    };

    //
    // Draw blocks
    //
    for (const auto& b : m.blocks) {
        glm::vec2 pos = project(b.x, b.z);
        if (pos.x < -200 || pos.x > ctx.screenWidth + 200) continue;

        float size = (b.size * 600.0f) / b.z;
        size = std::max(3.0f, std::min(size, 200.0f));

        QuadCmd q;
        q.position = {pos.x - size/2, pos.y - size};
        q.size = {size, size};
        q.color = b.color;
        q.tex = 0;
        out.quads.push_back(q);
    }

    //
    // Player reticle
    //
    {
        QuadCmd ch;
        ch.position = {ctx.screenWidth/2 - 10, ctx.horizon - 2};
        ch.size = {20, 4};
        ch.color = {1, 0, 0, 1};
        ch.tex = 0;
        out.quads.push_back(ch);
    }
}