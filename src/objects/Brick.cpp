/* Handles the Brick object /src/objects/Brick.cpp*/

#include "Brick.h"
#include <iostream>
#include <vector>

Brick::Brick(float x, float y, float w, float h, int durability, int scoreValue)
    : x(x), y(y), w(w), h(h), durability(durability), scoreValue(scoreValue), active(true) {
    std::cerr << "Brick created at (" << x << ", " << y << ")" << std::endl;
}

bool Brick::CheckCollision(float ballX, float ballY, float ballRadius) {
    if (!active) return false;
    if (ballX >= x - w/2 && ballX <= x + w/2 &&
        ballY + ballRadius >= y - h/2 && ballY - ballRadius <= y + h/2) {
        durability--;
        if (durability <= 0) {
            active = false;
            std::cerr << "Brick destroyed!" << std::endl;
        } else {
            std::cerr << "Brick hit! Durability: " << durability << std::endl;
        }
        return true;
    }
    return false;
}

int Brick::CountActive(const std::vector<Brick>& bricks) {
    int count = 0;
    for (const auto& brick : bricks) {
        if (brick.active) count++;
    }
    return count;
}