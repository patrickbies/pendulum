#pragma once

#include "renderer/Color.h"
#include "renderer/Renderer.h"
#include "physics/World.h"
#include "physics/Body.h"

#include <string_view>
#include <span>

struct Scene
{
    std::string_view name;
    void (*build)(World&);
};

std::span<const Scene> scenes();

inline void renderWorld(
    Renderer &renderer,
    const World &world)
{
    for (const Body &body : world.bodies())
    {
        renderer.circle(
            {static_cast<float>(body.position.x),
             static_cast<float>(body.position.y)},
            0.2f,
            Color::white());
    }

    for (const DistanceConstraint &constraint :
         world.constraints())
    {
        const Body &a = world.bodies().at(constraint.bodyA);
        const Body &b = world.bodies().at(constraint.bodyB);

        renderer.segment(
            toVec2f(a.position),
            toVec2f(b.position),
            0.04f,
            Color::white());
    }
}