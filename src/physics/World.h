#pragma once

#include <vector>
#include "math/Vec2.h"
#include "physics/Body.h"

using BodyId = std::uint32_t;

class World
{
public:
    BodyId createBody(const BodyDef& def);

    void step(float dt);

    Body& body(BodyId id);

private:
    Vec2d gravity_{0.0f, -9.81f};

    std::vector<Body> bodies_;
};