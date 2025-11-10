#pragma once
#include <span>
#include "CoreTypes.hpp"

struct EventData;
struct EventStack;

struct BaseModule {
    virtual ~BaseModule()=default;
    virtual void onEvent(const EventData& event);
    virtual void tick( float dt );
    virtual void extract( RenderSnapshots2D& s);
    virtual void initialize();
    virtual void onInit();
    bool iterateEnabled = true;
    bool renderEnabled = true;
    bool eventEnabled = true;
    uint8_t updateOrder = 128;   // 0..255 small sortable priority
    uint8_t drawOrder   = 128;
    bool isInitialized = false;
};
