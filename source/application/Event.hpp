#pragma once
enum class Event { Quit, Pause, Confirm, Back, MoveX, MoveY, MousePos, MouseWheel, MouseDown, MouseUp, WindowResized, FullscreenToggle, FrameRateToggle };

struct EventData
{
    Event action;
    const void*  data; 
};

struct F1   { float v; };
struct V2   { float x, y; };
struct I2   { int   a, b; };
struct Btn  { int   button; }; 
