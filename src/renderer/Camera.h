#pragma once

#include "math/Vec2.h"

class Camera
{
public:
    void setPosition(Vec2f position)
    {
        position_ = position;
    }

    void setViewHeight(float height)
    {
        viewHeight_ = height;
    }

    Vec2f position() const
    {
        return position_;
    }

    float viewHeight() const
    {
        return viewHeight_;
    }

private:
    Vec2f position_{0.0f, 0.0f};

    float viewHeight_ = 10.0f;
};