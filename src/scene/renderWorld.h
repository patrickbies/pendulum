#pragma once

#include "renderer/Color.h"
#include "renderer/Renderer.h"
#include "physics/World.h"
#include "physics/Body.h"

inline void renderWorld(
    Renderer &renderer,
    const World &world)
{
    for (const Body &body : world.bodies())
    {
        renderer.circle(
            {static_cast<float>(body.position.x),
             static_cast<float>(body.position.y)},
            0.25f,
            Color::white());
    }

    for (const DistanceConstraint &constraint :
         world.constraints())
    {
        const Body &a = *constraint.bodyA;

        const Body &b = *constraint.bodyB;

        renderer.segment(
            toVec2f(a.position),
            toVec2f(b.position),
            0.04f,
            Color::white());
    }
}