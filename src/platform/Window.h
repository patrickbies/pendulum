#pragma once

#include <SDL3/SDL.h>

class Window {
public:
    Window(
        const char* title,
        int width,
        int height
    );

    ~Window();

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    bool shouldClose() const
    {
        return shouldClose_;
    }

    SDL_Window* handle() const
    {
        return handle_;
    }

private:
    SDL_Window* handle_ = nullptr;

    bool shouldClose_ = false;
};