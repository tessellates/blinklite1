#pragma once
#include "Event.hpp"
#include <SDL3/SDL.h>

#include <vector>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include "Singleton.hpp"

struct EventStack : public Singleton<EventStack> {
    std::vector<EventData> out;
    std::vector<EventData> next;
    std::vector<std::byte> arena;
    std::vector<std::byte> nextArena;

    // State tracking
    bool axes_emitted = false;
    bool holdL=false, holdR=false, holdU=false, holdD=false;
    
    // Polling
    size_t pollIndex = 0;
    bool frameFinalized = false;

    // Arena allocator for current frame
    template<class T>
    const void* push(const T& v) {
        return _push(v, arena);
    }
    
    // Arena allocator for next frame (just in case)
    template<class T>
    const void* pushNext(const T& v) {
        return _push(v, nextArena);
    }

    template<class T> const void* _push(const T& v, std::vector<std::byte>& arenaVec) {
        const size_t align = alignof(T);
        size_t off = (arenaVec.size() + (align-1)) & ~(align-1);
        arenaVec.resize(off);
        
        const size_t start = arenaVec.size();
        arenaVec.resize(start + sizeof(T));
        std::memcpy(arenaVec.data() + start, &v, sizeof(T));
        return reinterpret_cast<const void*>(arenaVec.data() + start);
    }

    void emit_axes();
    void finalize_frame();
    
    // NEW: Polling interface
    bool poll(EventData& event);
    bool hasEvents();
    
    // OLD: Span interface (still available if needed)
    const std::vector<EventData>& flush();
    
    void reset();
};