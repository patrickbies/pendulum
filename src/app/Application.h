#pragma once

#include "platform/Window.h"
#include "renderer/Renderer.h"
#include "physics/World.h"
#include "renderer/Camera.h"

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
    Camera camera_;
    World world_;
};