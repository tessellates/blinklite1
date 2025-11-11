
#pragma once
#define ENGINE_DEFINE_SDL_APP(AppType)                              \
extern "C" SDL_AppResult SDL_AppInit(void** appstate, int, char**){                  \
    auto* app = new AppType();                                                       \
    if (!app->init()) return SDL_APP_FAILURE;                                   \
    *appstate = app;                                                                 \
    return SDL_APP_CONTINUE;                                                         \
}                                                                                    \
extern "C" SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* e){                 \
    auto* app = static_cast<AppType*>(appstate);                                     \
    if (e->type == SDL_EVENT_QUIT) return SDL_APP_SUCCESS;                           \
    app->onSDLEvent(*e);                                                                \
    return SDL_APP_CONTINUE;                                                         \
}                                                                                    \
extern "C" SDL_AppResult SDL_AppIterate(void* appstate){                             \
    static_cast<AppType*>(appstate)->iterate();                                      \
    return SDL_APP_CONTINUE;                                                         \
}                                                                                    \
extern "C" void SDL_AppQuit(void* appstate, SDL_AppResult r){                        \
    auto* app = static_cast<AppType*>(appstate);                                     \
    app->shutdown((int)r);                                                           \
    delete app;                                                                      \
}
