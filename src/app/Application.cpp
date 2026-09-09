#include "app/Application.h"
#include "scene/renderWorld.h"

#include <SDL3/SDL.h>

#include <iostream>
#include <chrono>

// temp spot for scenes:
void ballScene(World &world)
{
    constexpr double length = 1.5;

    BodyDef anchorDef;
    anchorDef.position = {0.0, 3.0};
    anchorDef.inverseMass = 0.0;

    const BodyId anchor =
        world.createBody(anchorDef);

    BodyDef bob1Def;
    bob1Def.position = {1.0, 2.0};
    bob1Def.inverseMass = 1.0;

    const BodyId bob1 =
        world.createBody(bob1Def);

    BodyDef bob2Def;
    bob2Def.position = {2.0, 1.0};
    bob2Def.inverseMass = 1.0;

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

bool Application::initialize()
{
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        std::cerr
            << "Failed to initialize SDL: "
            << SDL_GetError()
            << '\n';

        return false;
    }

    window_ = std::make_unique<Window>(
        "Pendulum",
        800,
        600);

    if (!renderer_.initialize(
            window_->handle()))
    {
        return false;
    }

    camera_.setViewHeight(10.0f);

    ballScene(world_);

    return true;
}

void Application::run()
{
    renderer_.setCamera(camera_);

    constexpr double physicsDt = 1.0 / 120.0;
    double accumulator = 0.0;

    auto previousTime =
        std::chrono::steady_clock::now();

    while (running_)
    {
        processEvents();

        const auto currentTime =
            std::chrono::steady_clock::now();

        double frameTime =
            std::chrono::duration<double>(
                currentTime - previousTime)
                .count();

        previousTime = currentTime;

        accumulator += frameTime;

        while (accumulator >= physicsDt)
        {
            world_.step(physicsDt);
            accumulator -= physicsDt;
        }

        if (!renderer_.beginFrame(Color::black()))
            continue;

        renderWorld(renderer_, world_);

        renderer_.endFrame();
    }
}

void Application::processEvents()
{
    SDL_Event event;

    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_EVENT_QUIT:
            running_ = false;
            break;

        case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
            running_ = false;
            break;

        case SDL_EVENT_MOUSE_BUTTON_DOWN:
        {
            if (event.button.button ==
                SDL_BUTTON_LEFT)
            {
                world_.beginDrag(
                    camera_.mouseWorldPosition(
                        event.button.x,
                        event.button.y, 800, 600));
            }

            break;
        }

        case SDL_EVENT_MOUSE_MOTION:
        {
            world_.updateDrag(
                camera_.mouseWorldPosition(
                    event.motion.x,
                    event.motion.y, 800, 600));

            break;
        }

        case SDL_EVENT_MOUSE_BUTTON_UP:
        {
            if (event.button.button ==
                SDL_BUTTON_LEFT)
            {
                world_.endDrag();
            }

            break;
        }

        default:
            break;
        }
    }
}

Application::~Application()
{
    renderer_.shutdown();

    window_.reset();

    SDL_Quit();
}