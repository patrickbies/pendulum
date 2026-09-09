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

    Vec2d mouseWorldPosition(
        float mouseX,
        float mouseY,
        int windowWidth,
        int windowHeight) const
    {
        if (windowWidth <= 0 || windowHeight <= 0)
            return {};

        const double aspect =
            static_cast<double>(windowWidth) /
            static_cast<double>(windowHeight);

        const double halfHeight =
            static_cast<double>(viewHeight_) * 0.5;

        const double halfWidth =
            halfHeight * aspect;

        const double normalizedX =
            2.0 * mouseX / windowWidth - 1.0;

        const double normalizedY =
            1.0 - 2.0 * mouseY / windowHeight;

        return {
            static_cast<double>(position_.x) +
                normalizedX * halfWidth,

            static_cast<double>(position_.y) +
                normalizedY * halfHeight};
    }

private:
    Vec2f position_{0.0f, 0.0f};

    float viewHeight_ = 10.0f;
};
