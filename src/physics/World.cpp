#include "physics/World.h"

void World::step(double dt)
{
    for (Body &body : bodies_)
    {
        if (body.inverseMass == 0.0 || body.mouseOnly)
            continue;

        body.velocity += gravity_ * dt;
    }

    for (int iteration = 0;
         iteration < 20;
         ++iteration)
    {
        for (auto &constraint : constraints_)
        {
            solveVelocityConstraint(constraint);
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

    std::vector<Vec2d> predictedPositions;
    predictedPositions.reserve(bodies_.size());

    for (const Body &body : bodies_)
        predictedPositions.push_back(body.position);

    for (int iteration = 0; iteration < 20; ++iteration)
    {
        if (dragConstraint_)
            solveDragConstraint(*dragConstraint_);

        for (auto &constraint : constraints_)
        {
            solvePositionConstraint(constraint);
        }
    }

    for (std::size_t i = 0; i < bodies_.size(); ++i)
    {
        Body &body = bodies_[i];

        if (body.inverseMass == 0.0)
            continue;

        body.velocity +=
            (body.position - predictedPositions[i]) / dt;
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
    body.inverseMass = def.inverseMass;
    body.inertia = def.inertia;
    body.mouseOnly = def.mouseOnly;

    bodies_.push_back(body);

    return static_cast<BodyId>(bodies_.size() - 1);
}

void World::createDistanceConstraint(
    BodyId a,
    BodyId b,
    double length)
{
    constraints_.push_back({.bodyA = a,
                            .bodyB = b,
                            .length = length});
}

// constraint solvers
void World::solveDragConstraint(
    const DragConstraint &constraint)
{
    Body &body =
        bodies_[constraint.body];

    if (body.inverseMass == 0.0 && !body.mouseOnly)
        return;

    body.position =
        constraint.target;
}

void World::beginDrag(Vec2d position)
{
    constexpr double pickRadius = 0.25;

    for (BodyId id = 0; id < bodies_.size(); ++id)
    {
        const Body &body = bodies_[id];

        if (body.inverseMass == 0.0 && !body.mouseOnly)
            continue;

        if (length(body.position - position) <= pickRadius)
        {
            dragConstraint_ = DragConstraint{
                .body = id,
                .target = body.position};

            return;
        }
    }
}

void World::updateDrag(Vec2d position)
{
    if (dragConstraint_)
        dragConstraint_->target = position;
}

void World::endDrag()
{
    dragConstraint_.reset();
}

void World::solveVelocityConstraint(
    DistanceConstraint &constraint)
{
    Body &a = bodies_[constraint.bodyA];
    Body &b = bodies_[constraint.bodyB];

    Vec2d delta =
        b.position - a.position;

    double distance =
        length(delta);

    Vec2d n =
        delta / distance;

    double relativeVelocity =
        dot(b.velocity - a.velocity, n);

    double effectiveInverseMass =
        a.inverseMass +
        b.inverseMass;

    if (effectiveInverseMass == 0.0)
        return;

    double lambda =
        -relativeVelocity /
        effectiveInverseMass;

    Vec2d impulse =
        lambda * n;

    a.velocity -=
        impulse * a.inverseMass;

    b.velocity +=
        impulse * b.inverseMass;
}

void World::solvePositionConstraint(
    DistanceConstraint &constraint)
{
    Body &a = bodies_[constraint.bodyA];
    Body &b = bodies_[constraint.bodyB];

    Vec2d delta =
        b.position - a.position;

    double distance =
        length(delta);

    Vec2d n =
        delta / distance;

    double error =
        distance - constraint.length;

    double inverseMass =
        a.inverseMass +
        b.inverseMass;

    if (inverseMass == 0.0)
        return;

    double lambda =
        -error / inverseMass;

    Vec2d correction =
        lambda * n;

    a.position -=
        correction * a.inverseMass;

    b.position +=
        correction * b.inverseMass;
}