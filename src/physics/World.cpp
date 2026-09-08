#include "physics/World.h"

void World::step(float dt)
{
    for (auto& body : bodies_)
    {
        body.position += body.velocity * dt;
        body.rotation += body.angularVelocity * dt;
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
    body.rotation = static_cast<float>(def.rotation);
    body.velocity = def.velocity;
    body.angularVelocity = static_cast<float>(def.angularVelocity);
    body.mass = def.mass;
    body.inertia = def.inertia;
    body.isStatic = def.isStatic;

    bodies_.push_back(body);

    return static_cast<BodyId>(bodies_.size() - 1);
}