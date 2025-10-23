/* Handles the Paddle object /src/objects/Paddle.cpp*/

#include "Paddle.h"
#include <SDL2/SDL.h>
#include <iostream>

void Paddle::Update(float deltaTime) {
    const uint8_t* keys = SDL_GetKeyboardState(NULL);
    if (keys[SDL_SCANCODE_A]) x -= 200 * deltaTime;  // Move left
    if (keys[SDL_SCANCODE_D]) x += 200 * deltaTime;  // Move right


    // Clamp to screen bounds
    if (x - w/2 < 0) x = w/2;
    if (x + w/2 > 800) x = 800 - w/2;

    // std::cerr << "Paddle pos: (" << x << ", " << y << ")" << std::endl;
}

void Paddle::SetWidth(float width) {
    this->w = width;
}