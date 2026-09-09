#include "app/Application.h"
#include "scene/Scene.h"

#include <SDL3/SDL.h>

#include <iostream>
#include <chrono>

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
    renderer_.setCamera(camera_);

    const Scene &scene =
        scenes()[1];

    scene.build(world_);

    return true;
}

void Application::run()
{
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