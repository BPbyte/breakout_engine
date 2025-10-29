/*For managing textures /src/util/Texture.h*/

#ifndef TEXTURE_H
#define TEXTURE_H
#include <GL/gl.h>

class Texture {
public:
    Texture(const char* imagePath);
    ~Texture();

    // Delete copy constructors
    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;

    // Move constructors
    Texture(Texture&& other) noexcept;
    Texture& operator=(Texture&& other) noexcept;

    void Bind() const;
    void Unbind() const;

private:
    GLuint id;
};

#endif