/* Handles the Ball object /src/objects/Ball.cpp*/
#include "Ball.h"
#include <iostream>
#include <algorithm>  // For clamp
#include <cmath>      // For sqrt (speed calc)

void Ball::Update(float deltaTime, float paddleX, float paddleY, float paddleW, float paddleH, Game& game) {
    x += vx * deltaTime;
    y += vy * deltaTime;
    // std::cerr << "Ball pos: (" << x << ", " << y << "), vel: (" << vx << ", " << vy << ")" << std::endl;
    
    // Wall collisions
    if (x - radius < 0 || x + radius > 800) vx = -vx;
    if (y - radius < 0) vy = -vy;
    if (y + radius > 600) {
        x = 400; y = 300; vy = -vy; // Reset
        game.LoseLife();
        std::cerr << "Ball lost!" << std::endl;
        if (game.lives <= 0) {
            std::cerr << "Game over! Final score: " << game.score << std::endl;
        }
    }

    // Paddle collision (reflection + subtle deflection + speed ramp)
    float paddleLeft = paddleX - paddleW / 2;
    float paddleRight = paddleX + paddleW / 2;
    float paddleTop = paddleY - paddleH / 2;

    if (x >= paddleLeft && x <= paddleRight &&  // Ball center in paddle x-range
        y + radius >= paddleTop && y <= paddleTop + 5.0f) {  // Tighter: Ball bottom grazes paddle top (within 5px for overlaps)
    
        // Calculate hit position (0 = left edge, 1 = right edge)
        float hitPos = (x - paddleLeft) / paddleW;
        hitPos = std::clamp(hitPos, 0.0f, 1.0f);
    
        // Preserve incoming speed, then ramp it slightly
        float incomingSpeed = std::sqrt(vx * vx + vy * vy);
        const float speedMultiplier = 1.02f;  // 2% boost per hit (tune: 1.01-1.05)
        const float maxSpeed = 400.0f;        // Cap to prevent chaos
        float newSpeed = incomingSpeed * speedMultiplier;
        if (newSpeed > maxSpeed) newSpeed = maxSpeed;
    
        // Reflect: Flip vy (upward bounce), keep vx base
        vy = -std::abs(vy);  // Ensures full upward strength
    
        // Subtle deflection: Nudge vx based on hitPos (small % impact)
        float deflectStrength = 100.0f;  // Tune: Higher = more edge curve (50-100 feels good)
        vx += (hitPos - 0.5f) * deflectStrength * 2.0f;  // -75 at left, 0 at center, +75 at right
    
        // Normalize to new ramped speed
        float tempSpeed = std::sqrt(vx * vx + vy * vy);
        if (tempSpeed > 0) {
            vx = (vx / tempSpeed) * newSpeed;
            vy = (vy / tempSpeed) * newSpeed;
        }
    
        // Position nudge to pop above paddle
        y = paddleTop - radius - 1.0f;
    
        game.AddPoint();
        std::cerr << "Paddle hit at pos " << hitPos << "! Speed ramped to: " << newSpeed 
                  << ", New vel: (" << vx << ", " << vy << ")" << std::endl;
    }
    if (y + radius > 600) {
        x = 400; y = 300; 
        vx = 200; vy = -200;  // Reset to base speeds (was vy = -vy; now explicit)
        game.LoseLife();
        std::cerr << "Ball lost! Reset to base speed." << std::endl;
        if (game.lives <= 0) {
            std::cerr << "Game over! Final score: " << game.score << std::endl;
        }
    }
}