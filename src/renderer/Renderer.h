#pragma once

#include "math/Vec2.h"
#include "renderer/Camera.h"
#include "renderer/Color.h"

#include <SDL3/SDL.h>

#include <vector>

class Renderer
{
public:
    Renderer() = default;
    ~Renderer();

    bool initialize(SDL_Window* window);
    void shutdown();

    bool beginFrame(Color clearColor);
    void endFrame();

    void setCamera(const Camera& camera);

    void circle(
        Vec2 center,
        float radius,
        Color color
    );

    void segment(
        Vec2 start,
        Vec2 end,
        float thickness,
        Color color
    );

private:
    struct CircleInstance
    {
        Vec2 center;
        float radius;
        float padding;
        Color color;
    };

    SDL_Window* window_ = nullptr;
    SDL_GPUDevice* device_ = nullptr;

    SDL_GPUCommandBuffer* commandBuffer_ = nullptr;
    SDL_GPUTexture* swapchainTexture_ = nullptr;

    Color clearColor_ = Color::black();

    Camera camera_;

    std::vector<CircleInstance> circles_;
};