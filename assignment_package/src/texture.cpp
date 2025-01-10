#include "texture.h"
#include "mygl.h"
#include <QOpenGLWidget>
#include <iostream>

Texture::Texture(MyGL& context)
    : mr_context(context), m_textureHandle(-1), mp_textureImage(nullptr)
{}

Texture::~Texture()
{}

void Texture::create(const char* texturePath) {
    mr_context.printGLErrorLog();

    QImage img(texturePath);
    // img.convertToFormat(QImage::Format_ARGB32);
    img = img.mirrored();
    mp_textureImage = std::make_shared<QImage>(img);
    mr_context.glGenTextures(1, &m_textureHandle);

    mr_context.printGLErrorLog();
}

void Texture::load(int texSlot) {
    mr_context.printGLErrorLog();

    mr_context.glActiveTexture(GL_TEXTURE0 + texSlot);

    // texture target is `GL_TEXTURE_2D` since we are working with 2D textures
    mr_context.glBindTexture(GL_TEXTURE_2D, GLuint(m_textureHandle));

    // These parameters need to be set for EVERY texture you create
    // They don't always have to be set to the values given here, but they do need
    // to be set
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    mr_context.glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
                          mp_textureImage->width(), mp_textureImage->height(),
                          0, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, mp_textureImage->bits());
    mr_context.printGLErrorLog();
}


void Texture::bind(int texSlot) {
    mr_context.glActiveTexture(GL_TEXTURE0 + texSlot);
    mr_context.glBindTexture(GL_TEXTURE_2D, m_textureHandle);
}
