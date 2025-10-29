/* Handles the Paddle object /src/objects/Paddle.h*/
#ifndef PADDLE_H
#define PADDLE_H

struct Paddle {
    float x, y;     // Position
    float w, h;     // Size
    float duration;
    float originalWidth; // Added to store default width

    void Update(float deltaTime);
    void SetWidth(float width);
    void SetDuration(float duration);
};

#endif