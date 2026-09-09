#pragma once

#include "physics/Body.h"

struct DistanceConstraint
{
    BodyId bodyA;
    BodyId bodyB;
    double length;
};