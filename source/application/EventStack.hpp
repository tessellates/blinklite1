#pragma once
#include "Event.hpp"
#include <SDL3/SDL.h>

#include <vector>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>

struct EventStack {
    std::vector<EventData> out;
    std::vector<std::byte> arena;
    bool holdL=false, holdR=false, holdU=false, holdD=false;

    // helpers
    template<class T>
    const void* push(const T& v){
        // align
        const size_t align = alignof(T);
        size_t off = (arena.size() + (align-1)) & ~(align-1);
        arena.resize(off);
        // store
        const size_t start = arena.size();
        arena.resize(start + sizeof(T));
        std::memcpy(arena.data() + start, &v, sizeof(T));
        return reinterpret_cast<const void*>(arena.data() + start);
    }

    void emit_axes();
    void finalize_frame();
    const std::vector<EventData>& flush();
    void reset(); // called when all events have been parsed (void* data is let go)
};