#pragma once

#include "physics/Body.h"

struct DistanceConstraint
{
    Body* bodyA;
    Body* bodyB;
    double length;
};