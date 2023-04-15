#pragma once

#include "drawable.h"

class ScreenQuadrangle : public Drawable
{
public:
    ScreenQuadrangle(OpenGLContext* context);
    virtual void createVBOdata();
};
