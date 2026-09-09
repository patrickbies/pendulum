#pragma once

#include <vector>
#include "math/Vec2.h"
#include "physics/Body.h"

using BodyId = std::uint32_t;

class World
{
public:
    BodyId createBody(const BodyDef &def);

    void step(double dt);

    Body &body(BodyId id);

    const std::vector<Body> &bodies() const
    {
        return bodies_;
    }

private:
    Vec2d gravity_{0.0f, -9.81};

    std::vector<Body> bodies_;
};