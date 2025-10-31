/* Handles the Ball object /src/objects/Ball.cpp*/
#include "Ball.h"
#include <iostream>
#include <algorithm>
#include <cmath>

void Ball::Update(float deltaTime, Paddle& paddle, Game& game) {
    x += vx * deltaTime;
    y += vy * deltaTime;
    
    // Wall collisions
    if (x - radius < 0 || x + radius > 800) vx = -vx;
    if (y - radius < 0) vy = -vy;
    if (y + radius > 600) {
        game.LoseLife();
        if (game.lives > 0) {
            // Reset to paddle
            float paddleTop = paddle.y - paddle.h / 2;
            x = paddle.x;
            y = paddleTop - radius - 1.0f;
            vx = 0;
            vy = 0;
            launched = false;  // Wait for spacebar launch
            std::cerr << "Ball lost! Reset to paddle. Lives: " << game.lives << std::endl;
        } else {
            std::cerr << "Game over! Final score: " << game.score << std::endl;
        }
        return;  // Skip further updates this frame
    }
    
    // Paddle collision (reflection + subtle deflection + speed ramp + big paddle count)
    float paddleLeft = paddle.x - paddle.w / 2;
    float paddleRight = paddle.x + paddle.w / 2;
    float paddleTop = paddle.y - paddle.h / 2;
    
    if (x >= paddleLeft && x <= paddleRight &&
        y + radius >= paddleTop && y <= paddleTop + 5.0f) {
        
        float hitPos = (x - paddleLeft) / paddle.w;
        hitPos = std::clamp(hitPos, 0.0f, 1.0f);
        
        // Preserve incoming speed, then ramp it slightly
        float incomingSpeed = std::sqrt(vx * vx + vy * vy);
        const float speedMultiplier = 1.02f;
        const float maxSpeed = 400.0f;
        float newSpeed = incomingSpeed * speedMultiplier;
        if (newSpeed > maxSpeed) newSpeed = maxSpeed;
        
        // Reflect: Flip vy
        vy = -std::abs(vy);
        
        // Subtle deflection
        float deflectStrength = 100.0f;
        vx += (hitPos - 0.5f) * deflectStrength * 2.0f;
        
        // Normalize to new speed
        float tempSpeed = std::sqrt(vx * vx + vy * vy);
        if (tempSpeed > 0) {
            vx = (vx / tempSpeed) * newSpeed;
            vy = (vy / tempSpeed) * newSpeed;
        }
        
        // Nudge position
        y = paddleTop - radius - 1.0f;
        
        game.AddPoint();
        
        // Big paddle hit count
        if (game.bigPaddleRemainingHits > 0) {
            game.bigPaddleRemainingHits--;
            std::cerr << "Big paddle hit! Remaining: " << game.bigPaddleRemainingHits << std::endl;
            if (game.bigPaddleRemainingHits <= 0) {
                paddle.w = paddle.originalWidth;
                std::cerr << "Big paddle expired!" << std::endl;
            }
        }
        
        std::cerr << "Paddle hit at pos " << hitPos << "! Speed ramped to: " << newSpeed 
                  << ", New vel: (" << vx << ", " << vy << ")" << std::endl;
    }
}