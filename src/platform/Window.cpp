#include "renderer/platform/Window.h"

#include <stdexcept>
#include <string>

Window::Window(
    const char* title,
    int width,
    int height
)
{
    handle_ = SDL_CreateWindow(
        title,
        width,
        height,
        SDL_WINDOW_RESIZABLE |
        SDL_WINDOW_HIGH_PIXEL_DENSITY
    );

    if (!handle_)
    {
        throw std::runtime_error(
            std::string("Failed to create window: ") +
            SDL_GetError()
        );
    }
}

Window::~Window()
{
    if (handle_)
    {
        SDL_DestroyWindow(handle_);
    }
}