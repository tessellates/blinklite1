// blink.hpp  (single public include)
#pragma once
#include <span>

struct RenderSnapshot2D;
struct EventData;

namespace blink {

struct AppDesc {
    int w=1280, h=720; const char* title="Blink"; bool vsync=true;
};

struct Scene {
    virtual ~Scene()=default;
    virtual void step(float dt, std::span<const EventData> ev, RenderSnapshot2D& out)=0;
    virtual void render(const RenderSnapshot2D& s)=0; // optional if you have your own backend
};

struct Layer {
    virtual ~Layer()=default;
    virtual void beginFrame(float /*dt*/, std::span<const EventData> /*ev*/) {}
    virtual void renderOverlay(const RenderSnapshot2D& /*s*/) {}
};

// builder
struct AppBuilder {
    AppDesc desc{};
    std::vector<std::unique_ptr<Layer>> layers;

    AppBuilder& set(const AppDesc& d){ desc=d; return *this; }
    template<class T, class...A> AppBuilder& with(A&&...a){
        layers.emplace_back(std::make_unique<T>(std::forward<A>(a)...));
        return *this;
    }
    template<class SceneT, class...A>
    int run(A&&...a); // defined in lib: creates engine, installs SceneT, layers, runs
};

} // namespace blink
