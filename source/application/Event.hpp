#pragma once
enum class Event {
    Quit, Pause, Confirm, Back, MoveX, MoveY, MousePos, MouseWheel, MouseDown, MouseUp, WindowResized, FullscreenToggle, FrameRateToggle,
    KeyDown, KeyUp
};

struct EventData
{
    Event action;
    const void*  data; 
};

enum MouseButton
{
    MOUSE_LEFT   = 0,
    MOUSE_MIDDLE = 1,
    MOUSE_RIGHT  = 2
};

struct F1   { float v; };
struct V2   { float x, y; };
struct I2   { int   a, b; };
struct Btn  { MouseButton button; float x, y; };

enum KeyboardKey {
    // Letters
    KEY_A, KEY_B, KEY_C, KEY_D, KEY_E, KEY_F, KEY_G, KEY_H, KEY_I, KEY_J, 
    KEY_K, KEY_L, KEY_M, KEY_N, KEY_O, KEY_P, KEY_Q, KEY_R, KEY_S, KEY_T, 
    KEY_U, KEY_V, KEY_W, KEY_X, KEY_Y, KEY_Z,
    
    // Numbers
    KEY_0, KEY_1, KEY_2, KEY_3, KEY_4, KEY_5, KEY_6, KEY_7, KEY_8, KEY_9,
    
    // Function keys
    KEY_F1, KEY_F2, KEY_F3, KEY_F4, KEY_F5, KEY_F6, KEY_F7, KEY_F8, KEY_F9, KEY_F10, KEY_F11, KEY_F12,
    
    // Special keys
    KEY_SPACE,
    KEY_ENTER,
    KEY_ESCAPE,
    KEY_BACKSPACE,
    KEY_TAB,
    
    // Arrow keys
    KEY_UP,
    KEY_DOWN,
    KEY_LEFT,
    KEY_RIGHT,
    
    // Modifiers
    KEY_LSHIFT,
    KEY_RSHIFT,
    KEY_CTRL,
    KEY_ALT,

    KEY_NONE
};