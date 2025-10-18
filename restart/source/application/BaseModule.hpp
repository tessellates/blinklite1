#pragma once
#include <span>
#include "CoreTypes.hpp"

struct EventData;
class Engine;
struct EventStack;

struct BaseModule {
    virtual ~BaseModule()=default;
    virtual void tick(float dt, std::span<const EventData> ev);
    virtual void extract( RenderSnapshots2D& s);
    virtual void initialize();
    virtual void onInit();
    bool iterateEnabled = true;
    bool renderEnabled = true;
    uint8_t updateOrder = 128;   // 0..255 small sortable priority
    uint8_t drawOrder   = 128;
    bool isInitialized = false;
};
