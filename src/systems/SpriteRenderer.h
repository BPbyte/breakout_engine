/* Renders sprites /src/systems/SpriteRenderer.h*/

#ifndef SPRITE_RENDERER_H
#define SPRITE_RENDERER_H

#include "../util/Texture.h"

class SpriteRenderer {
public:
    void DrawSprite(Texture& texture, float x, float y, float w, float h);
    void DrawNumber(Texture* digits, int number, float x, float y, float digitW = 16.0f, float digitH = 16.0f); 
};

#endif