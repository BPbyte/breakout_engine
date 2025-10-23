/* Renders sprites /src/systems/SpriteRenderer.cpp*/
#include "SpriteRenderer.h"
#include <string>

void SpriteRenderer::DrawSprite(Texture& texture, float x, float y, float w, float h) {
    texture.Bind();
    glBegin(GL_QUADS);
    glTexCoord2f(0, 1); glVertex2f(x-w/2, y-h/2); // Top-left (tex: bottom-left)
    glTexCoord2f(1, 1); glVertex2f(x+w/2, y-h/2); // Top-right (tex: bottom-right)
    glTexCoord2f(1, 0); glVertex2f(x+w/2, y+h/2); // Bottom-right (tex: top-right)
    glTexCoord2f(0, 0); glVertex2f(x-w/2, y+h/2); // Bottom-left (tex: top-left)
    glEnd();
    texture.Unbind();
}

void SpriteRenderer::DrawNumber(Texture* digits, int number, float x, float y, float digitW, float digitH) {
    if (number < 0) return;
    if (number == 0) {
        DrawSprite(digits[0], x, y, digitW, digitH);
        return;
    }
    std::string s = std::to_string(number);
    for (char c : s) {
        int d = c - '0';
        if (d < 0 || d > 9) {
            continue;
        }
        DrawSprite(digits[d], x, y, digitW, digitH);
        x += digitW;
    }
}