/* Handles the Brick object /src/objects/Brick.h*/

#ifndef BRICK_H
#define BRICK_H
#include <vector>

struct Brick {
    float x, y;     // Position
    float w, h;     // Size
    int durability; // Hits to destroy
    int scoreValue; // Points awarded
    bool active;    // Is brick alive?
    static int CountActive(const std::vector<Brick>& bricks);  // Declare static helper

    Brick(float x, float y, float w, float h, int durability, int scoreValue);
    bool CheckCollision(float ballX, float ballY, float ballRadius);
};

#endif