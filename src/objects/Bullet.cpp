// src/objects/bullet.cpp -for blaster powerup
#include "Bullet.h"
#include <algorithm>  // For clamp if needed, but simple overlap

bool Bullet::IsColliding(const Brick& brick) const {
    if (!active || !brick.active) return false;
    float bulletHalfW = 4.0f;  // 8x8 bullet
    float bulletHalfH = 4.0f;
    float brickLeft = brick.x - brick.w / 2;
    float brickRight = brick.x + brick.w / 2;
    float brickTop = brick.y - brick.h / 2;
    float brickBottom = brick.y + brick.h / 2;
    return x + bulletHalfW >= brickLeft && x - bulletHalfW <= brickRight &&
           y + bulletHalfH >= brickTop && y - bulletHalfH <= brickBottom;
}