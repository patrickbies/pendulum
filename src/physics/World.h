#pragma once

#include <vector>
#include <optional>
#include "math/Vec2.h"
#include "physics/Body.h"
#include "physics/DistanceConstraint.h"

using BodyId = std::uint32_t;

struct DragConstraint
{
    BodyId body;
    Vec2d target;
};

class World
{
public:
    BodyId createBody(const BodyDef &def);
    void createDistanceConstraint(
        BodyId a,
        BodyId b,
        double length);

    void step(double dt);

    Body &body(BodyId id);

    const std::vector<Body> &bodies() const
    {
        return bodies_;
    }

    const std::vector<DistanceConstraint> &constraints() const
    {
        return constraints_;
    }

    void beginDrag(Vec2d position);
    void updateDrag(Vec2d position);
    void endDrag();

private:
    Vec2d gravity_{0.0f, -9.81};

    std::optional<DragConstraint> dragConstraint_;

    std::vector<Body> bodies_;
    std::vector<DistanceConstraint> constraints_;
    
    void solveDragConstraint(const DragConstraint &constraint);
};
