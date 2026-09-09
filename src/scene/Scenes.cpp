#include "scene/Scene.h"

#include <array>
#include <vector>

namespace
{

void doublePendulum(World& world)
{
    constexpr double length = 1.5;

    BodyDef anchorDef;
    anchorDef.position = {0.0, 3.0};
    anchorDef.inverseMass = 0.0;
    anchorDef.mouseOnly = true;

    const BodyId anchor =
        world.createBody(anchorDef);

    BodyDef bob1Def;
    bob1Def.position = {1.0, 2.0};

    const BodyId bob1 =
        world.createBody(bob1Def);

    BodyDef bob2Def;
    bob2Def.position = {2.0, 1.0};

    const BodyId bob2 =
        world.createBody(bob2Def);

    world.createDistanceConstraint(
        anchor,
        bob1,
        length);

    world.createDistanceConstraint(
        bob1,
        bob2,
        length);
}

void cloth(World& world) {
    std::vector<BodyId> pr;
    double gap = 0.6;

    for (int i = 0; i < 15; i++) {
        std::vector<BodyId> row;
        BodyId p = -1;

        for (int j = 0; j < 15; j++) {
            BodyDef ad;
            ad.position = {(double) j * gap - 4.5, (double) i * gap - 4.5};
            if (i == 14) {
                ad.inverseMass = 0.0;
                ad.mouseOnly = true;
            }

            const BodyId a =
            world.createBody(ad);
            row.push_back(a);

            if (p != -1) {
                world.createDistanceConstraint(
                    p,
                    a,
                    gap);
            }
            if (i > 0) {
                world.createDistanceConstraint(
                    pr[j],
                    a,
                    gap);
            }

            p = a;
        }
        pr = row;
    }
}

const std::array sceneList{
    Scene{
        "Double Pendulum",
        doublePendulum
    },
    Scene{
        "Cloth",
        cloth
    }
};

}

std::span<const Scene> scenes()
{
    return sceneList;
}