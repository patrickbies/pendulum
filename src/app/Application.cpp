#include "app/Application.h"

#include <SDL3/SDL.h>

#include <iostream>

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

    return true;
}

void Application::run()
{
    while (running_)
    {
        processEvents();
        if (!renderer_.beginFrame())
        {
            continue;
        }

        // Render stuff here

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