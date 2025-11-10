#pragma once
#include "Event.hpp"
#include <SDL3/SDL.h>

#include <vector>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include "Singleton.hpp"

#include <memory_resource>
#include <vector>
#include <cstddef>
#include <cstring>
#include <algorithm>

struct EventStack : public Singleton<EventStack> {


    void pushNow( Event e ) {
        out.push_back({ e, nullptr });
    }

    void pushNext( Event e ) {
        next.push_back({ e, nullptr });
    }

    template<class T>
    void pushNow(Event e, const T& v) {
        out.push_back({ e, pushIntoArena(v, currentArena) });
    }

    template<class T>
    void pushNext(Event e, const T& v) {
        next.push_back({ e, pushIntoArena(v, nextArena) });
    }

private:

    struct Arena {
        std::pmr::monotonic_buffer_resource mbr;
        std::pmr::vector<std::byte> data{ &mbr };
        void clear() { data.clear(); mbr.release(); }
        void swap(Arena& other) noexcept {
            data.swap(other.data); // safe because both use their own mbr
    }
    };

    std::vector<EventData> out, next;
    Arena currentArena;
    Arena nextArena;

    template<class T>
    const void* pushIntoArena(const T& v, Arena& arena) {
        size_t off = (arena.data.size() + (alignof(T)-1)) & ~(alignof(T)-1);
        arena.data.resize(off + sizeof(T));
        std::memcpy(arena.data.data() + off, &v, sizeof(T));
        return arena.data.data() + off;
    }

public:
    // State tracking
    bool axes_emitted = false;
    bool holdL=false, holdR=false, holdU=false, holdD=false;
    
    // Polling
    size_t pollIndex = 0;
    bool frameFinalized = false;

    void emit_axes();
    void finalize_frame();
    
    // NEW: Polling interface
    bool poll(EventData& event);
    bool hasEvents();
    
    void reset();
};