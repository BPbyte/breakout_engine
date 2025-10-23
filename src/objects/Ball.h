/* Handles the Ball object /src/objects/Ball.h*/

#ifndef BALL_H
#define BALL_H

#include "../systems/Game.h"

struct Ball {
    float x, y;     // Position
    float vx, vy;   // Velocity
    float radius;   // Size

    void Update(float deltaTime, float paddleX, float paddleY, float paddleW, float paddleH, Game& game, bool& gameOver);
};

#endif