#pragma once

#include "drawable.h"

class Quad : public Drawable
{
public:
    Quad(MyGL& context);
    virtual void createVBOdata();
};
