/* Handles the Paddle object /src/objects/Paddle.cpp*/
#include "Paddle.h"
#include <SDL2/SDL.h>
#include <algorithm>
#include <iostream>

void Paddle::Update(float deltaTime) {
    const uint8_t* keys = SDL_GetKeyboardState(NULL);
    if (keys[SDL_SCANCODE_A]) x -= 350 * deltaTime;  
    if (keys[SDL_SCANCODE_D]) x += 350 * deltaTime;  

    // Update duration and revert width if powerup expires
    if (duration > 0) {
        duration -= deltaTime * 60.0f; 
        if (duration <= 0) {
            w = originalWidth; 
            duration = 0;
        }
    }

    // Clamp to screen bounds
    if (x - w/2 < 0) x = w/2;
    if (x + w/2 > 800) x = 800 - w/2;
}

void Paddle::SetWidth(float width) {
    const float maxWidth = 96.0f; // Maximum paddle width (1.5 * 64)
    this->w = std::min(width, maxWidth);
}

void Paddle::SetDuration(float Duration) {
    this->duration = Duration;
}