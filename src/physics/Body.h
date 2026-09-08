#pragma once

#include "math/Vec2.h"

struct BodyDef
{
    Vec2d position{0.0, 0.0};
    double rotation = 0.0;

    Vec2d velocity{0.0, 0.0};
    double angularVelocity = 0.0;

    double mass = 1.0;
    double inertia = 1.0;

    bool isStatic = false;
};

struct Body
{
    Vec2d position;
    float rotation = 0.0f;

    Vec2d velocity;
    float angularVelocity = 0.0f;

    double mass;
    double inertia;

    bool isStatic = false;
};
