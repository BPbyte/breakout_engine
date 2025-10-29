/* Handles the Ball object /src/objects/Ball.h*/

#ifndef BALL_H
#define BALL_H
#include "../systems/Game.h"
#include "Paddle.h"  // For ref
#include <algorithm>
#include <cmath>
struct Ball {
    float x, y; // Position
    float vx, vy; // Velocity
    float radius; // Base size
    void Update(float deltaTime, Paddle& paddle, Game& game);  // Now takes Paddle&
};
#endif