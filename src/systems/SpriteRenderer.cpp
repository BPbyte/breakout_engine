/* Renders sprites /src/systems/SpriteRenderer.cpp*/

#include "SpriteRenderer.h"

void SpriteRenderer::DrawSprite(Texture& texture, float x, float y, float w, float h) {
    texture.Bind();
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex2f(x-w/2, y-h/2);
    glTexCoord2f(1, 0); glVertex2f(x+w/2, y-h/2);
    glTexCoord2f(1, 1); glVertex2f(x+w/2, y+h/2);
    glTexCoord2f(0, 1); glVertex2f(x-w/2, y+h/2);
    glEnd();
    texture.Unbind();
}