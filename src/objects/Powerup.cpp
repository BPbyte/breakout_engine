#include "Powerup.h"
#include <iostream>

Powerup::Powerup(float x, float y, float w, float h, Type type)
    : x(x), y(y), w(w), h(h), vx(0), vy(100), type(type), active(true) {
    std::cerr << "Powerup created: " << (type == PADDLE_SIZE ? "Paddle Size" : "Extra Life") << std::endl;
}

void Powerup::Update(float deltaTime, float paddleX, float paddleY, float paddleW, float paddleH) {
    x += vx * deltaTime;
    y += vy * deltaTime; // Changed to += since y increases downward
    
    // Paddle collision
    if (x >= paddleX - paddleW/2 && x <= paddleX + paddleW/2 &&
        y >= paddleY - paddleH/2 && y <= paddleY + paddleH/2) {
        std::cerr << "Powerup hit!" << std::endl;
        active = false; // Deactivate powerup on hit
        // Add logic to apply powerup effect (e.g., increase paddle size or add life)
    }
    // Deactivate if powerup falls off screen
    if (y > 600) { // Assuming 600 is the screen height
        active = false;
    }
}