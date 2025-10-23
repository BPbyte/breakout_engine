/* Handles the Paddle object /src/objects/Paddle.h*/

#ifndef PADDLE_H
#define PADDLE_H

struct Paddle {
    float x, y;     // Position
    float w, h;     // Size

    void Update(float deltaTime);
};

#endif