#include "physics/World.h"

void World::step(double dt)
{
    for (Body &body : bodies_)
    {
        if (body.inverseMass == 0.0)
            continue;

        body.velocity += gravity_ * dt;
    }

    for (int iteration = 0;
         iteration < 10;
         ++iteration)
    {
        for (auto &constraint : constraints_)
        {
            Vec2d delta =
                constraint.bodyB->position - constraint.bodyA->position;

            double distance =
                length(delta);

            Vec2d n =
                delta / distance;

            double relativeVelocity =
                dot(constraint.bodyB->velocity - constraint.bodyA->velocity, n);

            double effectiveInverseMass =
                constraint.bodyA->inverseMass +
                constraint.bodyB->inverseMass;

            double lambda =
                -relativeVelocity /
                effectiveInverseMass;

            Vec2d impulse =
                lambda * n;

            constraint.bodyA->velocity -=
                impulse * constraint.bodyA->inverseMass;

            constraint.bodyB->velocity +=
                impulse * constraint.bodyB->inverseMass;
        }
    }

    for (auto &body : bodies_)
    {
        if (body.inverseMass == 0.0)
            continue;

        body.position +=
            body.velocity * dt;

        body.rotation +=
            body.angularVelocity * dt;
    }

    for (auto &constraint : constraints_)
    {
        Vec2d delta =
            constraint.bodyB->position - constraint.bodyA->position;

        double distance =
            length(delta);

        Vec2d n =
            delta / distance;

        double error =
            distance - constraint.length;

        double inverseMass =
            constraint.bodyA->inverseMass +
            constraint.bodyB->inverseMass;

        double lambda =
            -error / inverseMass;

        Vec2d correction =
            lambda * n;

        constraint.bodyA->position -=
            correction * constraint.bodyA->inverseMass;

        constraint.bodyB->position +=
            correction * constraint.bodyB->inverseMass;
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
    body.inverseMass = def.inverseMass == 0.0 ? 0.0 : 1.0 / def.inverseMass;
    body.inertia = def.inertia;

    bodies_.push_back(body);

    return static_cast<BodyId>(bodies_.size() - 1);
}

void World::createDistanceConstraint(
    BodyId a,
    BodyId b,
    double length)
{
    DistanceConstraint constraint;
    constraint.bodyA = &bodies_.at(a);
    constraint.bodyB = &bodies_.at(b);
    constraint.length = length;

    constraints_.push_back(constraint);
}
