#ifndef BRICK_H
#define BRICK_H

struct Brick {
    float x, y;     // Position
    float w, h;     // Size
    int durability; // Hits to destroy
    int scoreValue; // Points awarded
    bool active;    // Is brick alive?

    Brick(float x, float y, float w, float h, int durability, int scoreValue);
    bool CheckCollision(float ballX, float ballY, float ballRadius);
};

#endif