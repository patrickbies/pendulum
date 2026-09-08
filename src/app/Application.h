#pragma once

#include "renderer/platform/Window.h"
#include "renderer/Renderer.h"

#include <memory>

class Application 
{
public:
    bool initialize();
    void run();

    ~Application();

private:
    void processEvents();

    bool running_ = true;

    std::unique_ptr<Window> window_;

    Renderer renderer_;
};