#pragma once

#include "math/Vec2.h"

using BodyId = std::uint32_t;

struct BodyDef
{
    Vec2d position{0.0, 0.0};
    double rotation = 0.0;

    Vec2d velocity{0.0, 0.0};
    double angularVelocity = 0.0;

    double inverseMass = 1.0;
    double inertia = 1.0;

    bool mouseOnly = false;
};

struct Body
{
    Vec2d position;
    double rotation = 0.0f;

    Vec2d velocity;
    double angularVelocity = 0.0f;

    double inverseMass;
    double inertia;

    bool mouseOnly = false;
};
