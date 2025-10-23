/* Handles the Powerup object /src/objects/Powerup.cpp*/

#include "Powerup.h"
#include <iostream>

Powerup::Powerup(float x, float y, float w, float h, Type type)
    : x(x), y(y), w(w), h(h), type(type), active(true) {
    std::cerr << "Powerup created: " << (type == PADDLE_SIZE ? "Paddle Size" : "Extra Life") << std::endl;
}
