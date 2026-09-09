#include "physics/World.h"

void World::step(double dt)
{
    for (auto &body : bodies_)
    {
        if (body.isStatic)
            continue;

        body.velocity += gravity_ * dt;

        body.position +=
            body.velocity * dt;

        body.rotation +=
            body.angularVelocity * dt;
    }
}

Body &World::body(BodyId id)
{
    return bodies_.at(id);
}

BodyId World::createBody(const BodyDef &def)
{
    Body body;
    body.position = def.position;
    body.rotation = def.rotation;
    body.velocity = def.velocity;
    body.angularVelocity = def.angularVelocity;
    body.mass = def.mass;
    body.inertia = def.inertia;
    body.isStatic = def.isStatic;

    bodies_.push_back(body);

    return static_cast<BodyId>(bodies_.size() - 1);
}