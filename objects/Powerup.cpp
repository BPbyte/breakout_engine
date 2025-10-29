/* Handles the Powerup object /src/objects/Powerup.cpp*/
#include "Powerup.h"
#include <iostream>

Powerup::Powerup(float x, float y, float w, float h, Type type)
    : x(x), y(y), w(w), h(h), vx(0), vy(100), type(type), active(true), collected(false) {
    std::cerr << "Powerup created: " << (type == PADDLE_SIZE ? "Paddle Size" : "Extra Life") << std::endl;
}

void Powerup::Update(float deltaTime, float paddleX, float paddleY, float paddleW, float paddleH) {
    x += vx * deltaTime;
    y += vy * deltaTime;
    
    // Paddle collision
    if (x >= paddleX - paddleW/2 && x <= paddleX + paddleW/2 &&
        y >= paddleY - paddleH/2 && y <= paddleY + paddleH/2) {
        std::cerr << "Powerup hit!" << std::endl;
        active = false; // Deactivate powerup on hit
        collected = true;
    }
    // Deactivate if powerup falls off screen
    if (y > 600) { 
        active = false;
        collected = false;
    }
}