#include "app/Application.h"
#include "scene/renderWorld.h"

#include <SDL3/SDL.h>

#include <iostream>

// temp spot for scenes:
void ballScene(World &world)
{
    BodyDef ball;

    ball.position = {0.0, 3.0};
    ball.mass = 1.0;

    world.createBody(ball);
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

    world_ = World();
    ballScene(world_);

    return true;
}

void Application::run()
{
    Camera camera;
    camera.setViewHeight(10.0f);

    renderer_.setCamera(camera);

    while (running_)
    {
        processEvents();

        if (!renderer_.beginFrame(
                Color::black()))
        {
            continue;
        }

        renderWorld(
            renderer_,
            world_);

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