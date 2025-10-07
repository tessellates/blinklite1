#pragma once
#include <functional>
#include "CoreTypes.hpp"
#include <span>
#include "Action.hpp"

class Engine {
public:
    bool init(const EngineConfig&);
    //int  run(const GameCallbacks&);
    void quit(); // request exit
    void iterate(double dt, std::span<const Actions> actions); // one frame
    void setCallbacks(const GameCallbacks& cbs) { callbacks = cbs; }
    void* getRenderer();
private:
    //bool pumpEvents(const GameCallbacks&);
    struct Impl; Impl* p_=nullptr;
    GameCallbacks callbacks;
    RenderSnapshot2D renderSnapshot;
    
};