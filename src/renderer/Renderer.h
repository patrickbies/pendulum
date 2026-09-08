#pragma once

#include "renderer/FrameConfig.h"
#include "renderer/renderer2d/Renderer2D.h"

#include <SDL3/SDL.h>

class Renderer
{
public:
    Renderer() = default;
    ~Renderer();

    bool initialize(SDL_Window* window);
    void shutdown();

    bool beginFrame(const FrameConfig& config);
    void endFrame();

    Renderer2D& draw2D()
    {
        return renderer2D_;
    }

private:
    SDL_Window* window_ = nullptr;
    SDL_GPUDevice* device_ = nullptr;

    SDL_GPUCommandBuffer* commandBuffer_ = nullptr;
    SDL_GPUTexture* swapchainTexture_ = nullptr;

    FrameConfig frameConfig_{};

    Renderer2D renderer2D_;
};