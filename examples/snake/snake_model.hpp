#pragma once
#include <deque>
#include <glm/vec2.hpp>
#include <iostream>

#include <random>
inline float frand_10() {
    static thread_local std::mt19937 gen(std::random_device{}());
    std::uniform_real_distribution<float> dist(0.f, 1.f);
    return 1.f - dist(gen);
}

enum Dir { Up, Down, Left, Right, None };

struct SnakeModel {
    int W=16, H=12;
    std::deque<glm::ivec2> body{{16,12},{15,12},{14,12}};
    glm::ivec2 food{8,8};
    Dir dir=Right;
    bool alive=true;
    float acc=0.f;        // time accumulator
    float step_s=240.0f;   // snake speed (seconds/tile)
    Dir buffer=None; // buffered dir input
    bool reponsed=false;
};

struct SnakeGrid 
{
    int w=16;
    int h=12;
    int margins=1;
};

struct SnakeContext
{
    float cell;
    float xmargins;
    float ymargins;
    SnakeContext(const SnakeGrid& g, float targetW, float targetH)
    {
        cell = std::min( (targetW) / g.w , (targetH) / g.h );
        float margins = cell * g.margins;
        cell = std::min( (targetW - 2*margins) / g.w , (targetH - 2*margins) / g.h );
        xmargins = (targetW - cell * g.w) / 2.0f;
        ymargins = (targetH - cell * g.h) / 2.0f;
        std::cout << "SnakeContext: cell=" << cell << " margins=" << margins << std::endl;
        std::cout << "SnakeContext: cell=" << cell << " xmargins=" << xmargins << std::endl;
        std::cout << "SnakeContext: cell=" << cell << " ymargins=" << ymargins << std::endl;
    }
};

#include <Event.hpp>

inline bool snake_step(SnakeModel& m, float dt){
    if (!m.alive) return false;
    m.acc += dt;
    bool moved = false;
    bool speedup= false;
    while (m.acc >= m.step_s || (false && m.buffer != None)) {// || (m.buffer != None && m.acc > m.step_s/3)) {
        if (m.buffer!=None){
            m.dir = m.buffer;
            m.buffer = None;
            if (!m.reponsed)
                if (m.acc < m.step_s)
                    speedup = true;
        }
        if (!m.reponsed) 
        {
            m.reponsed = true;
        }
        if (m.acc >= m.step_s)
        {
            if (m.reponsed)
                m.reponsed = false;
        }
        if (!speedup)
        {
            m.acc -= m.step_s;
            speedup = false;
        }
        else
        {
            m.acc -= 1.5*m.step_s;
        }
        auto head = m.body.front();
        if (m.dir==Up)    head.y -= 1;
        if (m.dir==Down)  head.y += 1;
        if (m.dir==Left)  head.x -= 1;
        if (m.dir==Right) head.x += 1;

        // wrap (or kill on wall)
        head.x = (head.x + m.W) % m.W;
        head.y = (head.y + m.H) % m.H;

        // self-hit?
        for (auto& p: m.body) if (p==head) { m.alive=false; return false; }

        m.body.push_front(head);
        if (head==m.food){
            // spawn new food (naive)
            m.food = { (head.x*7 + 3) % m.W, (head.y*11 + 5) % m.H };
        } else {
            m.body.pop_back();
        }
        moved = true;
    }
    return moved;
}

#include <CoreTypes.hpp>
inline void snake_extract(const SnakeModel& m, SnakeContext context, RenderSnapshot2D& out, bool triggerExtract){
    if (!triggerExtract) return;
    out.quads.clear();
    //out.context.viewport = {0, 0, context.w, context.h};
    const auto& cell = context.cell;
    const auto& xmargins = context.xmargins;
    const auto& ymargins = context.ymargins;
    
    auto push_rect = [&](float x, float y, glm::vec4 color){
        QuadCmd q;
        q.position = {x + xmargins, y + ymargins};
        q.size = {cell, cell};
        q.rotation = 0.0f;
        q.tex = 0;  // No texture, colored quad
        q.color = color;
        q.uv = {0, 0, 1, 1};  // Ignored since tex == 0
        q.sortKey = 0;
        out.quads.push_back(q);
    };
    
    // Snake body
    for (size_t i = 0; i < m.body.size(); ++i) {
        auto p = m.body[i];
        glm::vec4 color = i ? glm::vec4(0, (float)m.body.size()/45, 0, 1) : glm::vec4(0, 1, 0, 1);
        push_rect(p.x * cell, p.y * cell, color);
    }
    
    // Food
    push_rect(m.food.x * cell, m.food.y * cell, glm::vec4(1, 0, 0, 1));
}

inline void snake_handle_actions(SnakeModel& m, const EventData& in){
    if (in.action==Event::Confirm && !m.alive)
            m = SnakeModel{};
    if (m.buffer == None)
    {
        if (in.action==Event::MoveX){
            float x = static_cast<const F1*>(in.data)->v;
            if (x>0.5f && m.dir!=Left && m.dir!=Right)  m.buffer=Right;
            if (x<-0.5f && m.dir!=Right && m.dir!=Left) m.buffer=Left;
        } else if (in.action==Event::MoveY){
            float y = static_cast<const F1*>(in.data)->v;
            if (y>0.5f && m.dir!=Up  && m.dir!= Down)    m.buffer=Down;
            if (y<-0.5f && m.dir!=Down && m.dir!=Up) m.buffer=Up;
        } 
    }
}

struct SnakeGame {
    SnakeModel model{};
    bool moved = false;
    SnakeContext context{ {16,12,1}, 800.0f, 600.0f };
    void onEvent(const EventData& event)
    {
        snake_handle_actions(model, event);
    }

    void tick(float dt)
    {
        moved = snake_step(model, dt);
    }
    void applyWindowSize(int w, int h){
        context = SnakeContext{ {16,12,1}, (float)w, (float)h };
    }
};
