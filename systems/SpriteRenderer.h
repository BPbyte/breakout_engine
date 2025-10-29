/* Renders sprites /src/systems/SpriteRenderer.h*/

#ifndef SPRITE_RENDERER_H
#define SPRITE_RENDERER_H

#include "../util/Texture.h"
#include <string>

class SpriteRenderer {
public:
    void DrawSprite(Texture& texture, float x, float y, float w, float h);
    void DrawNumber(Texture* digits, int number, float x, float y, float digitW = 16.0f, float digitH = 16.0f); 
    void DrawText(Texture& texture, float x, float y, float w, float h, const std::string& text);  // Stub for future font, but for now we can hack with digits.
};

#endif