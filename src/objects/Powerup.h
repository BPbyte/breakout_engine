#ifndef POWERUP_H
#define POWERUP_H

struct Powerup {
    float x, y;     // Position
    float w, h;     // Size
    bool active;    // Is active?
    enum Type { NONE, PADDLE_SIZE, EXTRA_LIFE } type;

    Powerup(float x, float y, float w, float h, Type type);
};

#endif