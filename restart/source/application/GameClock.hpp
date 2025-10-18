#pragma once

#include <cstdint>

class GameClock {
public:
    GameClock();
    GameClock(float gameSpeed);
    void update(); // Call this once per frame
    void reset(); 
    float getDeltaTime() const; // Get the time elapsed since last frame in seconds
    float getIntervalTime() const; // Get the time elapsed since last frame in seconds
    float getIntervalProgress() const; 
    float getProgress() const;
    void setGameSpeed(float gameSpeed); // Get the time elapsed since last frame in seconds
    float getGameSpeed() const; // Get the time elapsed since last frame in seconds
    uint32_t getTotalTime() const; // Get the total elapsed time since the start of the clock in milliseconds

private:
    uint32_t lastTick; // Time of the last update call
    uint32_t startTick; // The time when the clock was started
    uint32_t intervalTick; // The time when the clock was started
    float gameSpeed; // The time when the clock was started
    float deltaTime; // Time between the current frame and the last frame in seconds
    float intervalTime;// Time between
};
