/*For managing textures /src/util/Texture.cpp*/


#include "Texture.h"
#include <iostream>
#include <stb_image.h>

Texture::Texture(const char* imagePath) {
    std::cerr << "Loading texture: " << imagePath << std::endl;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(imagePath, &width, &height, &nrChannels, 0);

    if (data) {
        std::cerr << "Loaded " << imagePath << ": " << width << "x" << height << ", channels=" << nrChannels << std::endl;
        GLenum format;
        switch (nrChannels) {
            case 1: format = GL_RED; break;
            case 3: format = GL_RGB; break;
            case 4: format = GL_RGBA; break;
            default: std::cerr << "Unsupported image format: " << nrChannels << std::endl; return;
        }

        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        GLenum err = glGetError();
        if (err != GL_NO_ERROR) std::cerr << "OpenGL error: " << err << std::endl;
    } else {
        std::cerr << "Failed to load texture: " << imagePath << ". Reason: " << stbi_failure_reason() << std::endl;
    }

    stbi_image_free(data);
}

Texture::~Texture() {
    glBindTexture(GL_TEXTURE_2D, 0);
    glDeleteTextures(1, &id);
}

void Texture::Bind() const {
    glBindTexture(GL_TEXTURE_2D, id);
}

void Texture::Unbind() const {
    glBindTexture(GL_TEXTURE_2D, 0);
}