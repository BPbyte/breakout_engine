/* Handles the Brick object /src/objects/Brick.cpp*/

#include "Brick.h"
#include <iostream>

Brick::Brick(float x, float y, float w, float h, int durability, int scoreValue)
    : x(x), y(y), w(w), h(h), durability(durability), scoreValue(scoreValue), active(true) {
    std::cerr << "Brick created at (" << x << ", " << y << ")" << std::endl;
}

bool Brick::IsColliding(float ballX, float ballY, float ballRadius) {
    if (!active) return false;
    return ballX >= x - w/2 && ballX <= x + w/2 &&
           ballY + ballRadius >= y - h/2 && ballY - ballRadius <= y + h/2;
}

int Brick::CountActive(const std::vector<Brick>& bricks) {
    int count = 0;
    for (const auto& brick : bricks) {
        if (brick.active) count++;
    }
    return count;
}