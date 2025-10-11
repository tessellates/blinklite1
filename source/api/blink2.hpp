// blink.hpp  (single public include)
#pragma once
#include <span>

struct RenderSnapshot2D;
struct EventData;

namespace blink {

struct AppDesc {
    int w=1280, h=720; const char* title="Blink"; bool vsync=true;
};

struct BaseModule {
    virtual ~BaseModule()=default;
    virtual void iteration(float dt, std::span<const EventData> ev);
    virtual void populateRenderSnapshot(const RenderSnapshot2D& s);
    virtual void initialize(EngineAppBase* baseApp);
    bool iterateEnabled = true;
    bool renderEnabled = true;
};

// builder
struct AppBuilder {
    AppDesc desc{};
    std::vector<std::unique_ptr<BaseModule>> modules;

    AppBuilder& set(const AppDesc& d){ desc=d; return *this; }
    template<class T, class...A> AppBuilder& with(A&&...a){
        entities.emplace_back(std::make_unique<T>(std::forward<A>(a)...));
        return *this;
    }
    template<class SceneT, class...A>
    int run(A&&...a); // defined in lib: creates engine, installs SceneT, layers, runs
};

} // namespace blink
