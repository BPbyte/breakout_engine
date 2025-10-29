/* Handles the Powerup object /src/objects/Powerup.h*/

#ifndef POWERUP_H
#define POWERUP_H
#include "../systems/Game.h"
#include "Ball.h"
#include "Paddle.h"
struct Powerup {
    float x, y; // Position
    float w, h; // Size
    float vx, vy; // Velocity
    bool active; // Is active?
    bool collected; // Was collected?
    float duration;  // Unused now, but kept for future
    enum Type { NONE, PADDLE_SIZE, EXTRA_LIFE, METEOR, BIG_BALL, BLASTER } type;
    Powerup(float x, float y, float w, float h, Type type);
    void Update(float deltaTime, Paddle& paddle, Ball& ball, Game& game);  // Now takes refs
};
#endif