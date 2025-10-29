/* Handles the Powerup object /src/objects/Powerup.cpp*/
#include "Powerup.h"
#include <iostream>

Powerup::Powerup(float x, float y, float w, float h, Type type)
    : x(x), y(y), w(w), h(h), vx(0), vy(100), type(type), active(true), collected(false), duration(0) {
    std::cerr << "Powerup created: ";
    switch (type) {
        case PADDLE_SIZE: std::cerr << "Big Paddle"; break;
        case EXTRA_LIFE: std::cerr << "Extra Life"; break;
        case METEOR: std::cerr << "Meteor"; break;
        case BIG_BALL: std::cerr << "Big Ball"; break;
        case BLASTER: std::cerr << "Blaster"; break;
        default: std::cerr << "None";
    }
    std::cerr << std::endl;
}

void Powerup::Update(float deltaTime, Paddle& paddle, Ball& ball, Game& game) {
    x += vx * deltaTime;
    y += vy * deltaTime;
   
    // Paddle collision (rect-rect)
    float px = paddle.x, py = paddle.y, pw = paddle.w, ph = paddle.h;
    if (x >= px - pw/2 && x <= px + pw/2 &&
        y >= py - ph/2 && y <= py + ph/2) {
        std::cerr << "Powerup collected: ";
        switch (type) {
            case PADDLE_SIZE: std::cerr << "Big Paddle"; break;
            case EXTRA_LIFE: std::cerr << "Extra Life"; break;
            case METEOR: std::cerr << "Meteor"; break;
            case BIG_BALL: std::cerr << "Big Ball"; break;
            case BLASTER: std::cerr << "Blaster"; break;
        }
        std::cerr << std::endl;
        active = false;
        collected = true;
    }
    // Deactivate if off screen
    if (y > 600) {
        active = false;
        collected = false;
    }
}