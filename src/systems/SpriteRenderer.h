#ifndef SPRITE_RENDERER_H
#define SPRITE_RENDERER_H

#include "../util/Texture.h"

class SpriteRenderer {
public:
    void DrawSprite(Texture& texture, float x, float y, float w, float h);
};

#endif