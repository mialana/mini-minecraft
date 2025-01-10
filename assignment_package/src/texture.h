#pragma once

#include <memory>
#include <QImage>

class MyGL;

class Texture
{
public:
    Texture(MyGL& context);
    ~Texture();

    void create(const char *texturePath);
    void load(int texSlot);
    void bind(int texSlot);

private:
    MyGL& mr_context;
    unsigned int m_textureHandle;
    std::shared_ptr<QImage> mp_textureImage;
};
