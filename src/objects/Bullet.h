// src/objects/Bullet.h -For blaster powerup
#ifndef BULLET_H
#define BULLET_H
#include "Brick.h"  // For collision
struct Bullet {
    float x, y; // Position
    float vx, vy; // Velocity (straight down)
    bool active;
    Bullet(float px, float py, float vx = 0.0f, float vy = -300.0f) : x(px), y(py), vx(vx), vy(vy), active(true) {}
    bool IsColliding(const Brick& brick) const;  // Rect-rect overlap
};
#endif