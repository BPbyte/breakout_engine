/* Handles the Ball object /src/objects/Ball.cpp*/

#include "Ball.h"
#include <iostream>

void Ball::Update(float deltaTime, float paddleX, float paddleY, float paddleW, float paddleH, Game& game) {
    x += vx * deltaTime;
    y += vy * deltaTime;

    // std::cerr << "Ball pos: (" << x << ", " << y << "), vel: (" << vx << ", " << vy << ")" << std::endl;

    // Wall collisions
    if (x - radius < 0 || x + radius > 800) vx = -vx;
    if (y - radius < 0) vy = -vy;
    if (y + radius > 600) {
        x = 400; y = 300; vy = -vy;  // Reset
        game.LoseLife();
        std::cerr << "Ball lost!" << std::endl;
        if (game.lives <= 0) { 
            std::cerr << "Game over! Final score: " << game.score << std::endl;
        }
    }

    // Paddle collision
    if (x >= paddleX - paddleW/2 && x <= paddleX + paddleW/2 &&
        y + radius >= paddleY - paddleH/2 && y - radius <= paddleY + paddleH/2) {
        vy = -vy;
        game.AddPoint();  // This adds 1 point per paddle hit;
        std::cerr << "Paddle hit!" << std::endl;
    }
}