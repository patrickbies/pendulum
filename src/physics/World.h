#pragma once

#include <vector>
#include "math/Vec2.h"
#include "physics/Body.h"
#include "physics/DistanceConstraint.h"

using BodyId = std::uint32_t;

class World
{
public:
    BodyId createBody(const BodyDef &def);
    void createDistanceConstraint(
        BodyId a,
        BodyId b,
        double length);

    void step(double dt);

    Body &body(BodyId id);

    const std::vector<Body> &bodies() const
    {
        return bodies_;
    }

    const std::vector<DistanceConstraint> &constraints() const
    {
        return constraints_;
    }

private:
    Vec2d gravity_{0.0f, -9.81};

    std::vector<Body> bodies_;
    std::vector<DistanceConstraint> constraints_;
};