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
    int W=32, H=24;
    std::deque<glm::ivec2> body{{16,12},{15,12},{14,12}};
    glm::ivec2 food{8,8};
    Dir dir=Right;
    bool alive=true;
    float acc=0.f;        // time accumulator
    float step_s=120.0f;   // snake speed (seconds/tile)
    Dir buffer=None; // buffered dir input
    Dir buffer2=None; // buffered dir input
};

#include <Action.hpp>
inline void snake_handle_actions(SnakeModel& m, std::span<const Actions> in){
    for (auto& a: in){
        if (a.action==Action::Confirm && !m.alive)
                m = SnakeModel{};
        if (m.buffer == None)
        {
            if (a.action==Action::MoveX){
                float x = static_cast<const F1*>(a.data)->v;
                if (x>0.5f && m.dir!=Left && m.dir!=Right)  m.buffer=Right;
                if (x<-0.5f && m.dir!=Right && m.dir!=Left) m.buffer=Left;
            } else if (a.action==Action::MoveY){
                float y = static_cast<const F1*>(a.data)->v;
                if (y>0.5f && m.dir!=Up  && m.dir!= Down)    m.buffer=Down;
                if (y<-0.5f && m.dir!=Down && m.dir!=Up) m.buffer=Up;
            } 
        }
        else if (m.buffer2 == None)
        {
            if (a.action==Action::MoveX)
            {
                float x = static_cast<const F1*>(a.data)->v;
                if (x>0.5f && m.buffer!=Left)  m.buffer2=Right;
                if (x<-0.5f && m.buffer!=Right) m.buffer2=Left;
            } else if (a.action==Action::MoveY)
            {
                float y = static_cast<const F1*>(a.data)->v;
                if (y>0.5f && m.buffer!=Up)    m.buffer2=Down;
                if (y<-0.5f && m.buffer!=Down) m.buffer2=Up;
            } 
        }
    }
}

inline bool snake_step(SnakeModel& m, float dt){
    if (!m.alive) return false;
    m.acc += dt;
    bool moved = false;
    while (m.acc >= m.step_s){
        if (m.buffer!=None){
            m.dir = m.buffer;
            m.buffer = None;
            //m.buffer2 = None;
        }
        m.acc -= m.step_s;
        if (m.acc >= m.step_s)
            m.acc = 0; // avoid spiral of death
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
inline void snake_extract(const SnakeModel& m, float cell, RenderSnapshot2D& out, bool& moved){
    if (!moved) return;
    out.quads.clear();
    auto push_rect = [&](float x,float y, glm::vec4 color){
        QuadCmd q;
        q.mvp = glm::mat4(1.0f);            // pixel-space, no transform
        q.uv  = {0,0,1,1};
        q.color = color;
        // encode rect via MVP rows (cheap): scale+translate
        q.mvp[0][0]=cell; q.mvp[1][1]=cell; q.mvp[3][0]=x; q.mvp[3][1]=y;
        out.quads.push_back(q);
    };
    // grid background (optional)
    // snake
    for (size_t i=0;i<m.body.size();++i){
        auto p = m.body[i];
        push_rect(p.x*cell, p.y*cell, i? glm::vec4((float)m.body.size()/100, 0,(float)m.body.size()/100, 1) : glm::vec4(0,1,0,1));
    }
    // food
    push_rect(m.food.x*cell, m.food.y*cell, glm::vec4(1,0,0,1));
}

struct SnakeGame {
    SnakeModel model{};
    float cell = 24.f; // pixels per cell
    void step(float dt, std::span<const Actions> in, RenderSnapshot2D& out){
        snake_handle_actions(model, in);
        bool moved = snake_step(model, dt);
        snake_extract(model, cell, out, moved);
    }
};