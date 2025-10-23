#ifndef TEXTURE_H
#define TEXTURE_H
#include <GL/gl.h>

class Texture {
public:
    Texture(const char* imagePath);
    ~Texture();

    void Bind() const;
    void Unbind() const;

private:
    GLuint id;
};

#endif
