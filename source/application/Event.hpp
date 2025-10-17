#pragma once
enum class Event { Quit, Pause, Confirm, Back, MoveX, MoveY, MousePos, MouseWheel, MouseDown, MouseUp, WindowResized, FullscreenToggle, FrameRateToggle };

struct EventData
{
    Event action;
    const void*  data; 
};

enum MouseButton
{
    LEFT   = 0,
    MIDDLE = 1,
    RIGHT  = 2
};

struct F1   { float v; };
struct V2   { float x, y; };
struct I2   { int   a, b; };
struct Btn  { MouseButton button; float x, y; }; 
