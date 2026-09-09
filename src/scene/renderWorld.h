#pragma once

#include "renderer/Color.h"
#include "renderer/Renderer.h"
#include "physics/World.h"
#include "physics/Body.h"

void renderWorld(
    Renderer& renderer,
    const World& world)
{
    for (const Body& body : world.bodies())
    {
        renderer.circle(
            {
                static_cast<float>(body.position.x),
                static_cast<float>(body.position.y)
            },
            0.25f,
            Color::white()
        );
    }
}