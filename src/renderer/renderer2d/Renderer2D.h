#pragma once

#include "math/Vec2.h"
#include "renderer/Color.h"
#include "renderer/renderer2d/Camera2D.h"

#include <SDL3/SDL.h>

#include <cstddef>
#include <vector>

class Renderer2D
{
public:
    bool initialize(
        SDL_GPUDevice* device,
        SDL_GPUTextureFormat targetFormat
    );

    void shutdown();

    void beginFrame();

    void setCamera(
        const Camera2D& camera
    );

    void circle(
        Vec2 center,
        float radius,
        Color color
    );

private:
    friend class Renderer;

    struct CircleInstance
    {
        Vec2 center;

        float radius;
        float padding;

        Color color;
    };

    void upload(
        SDL_GPUCommandBuffer* commandBuffer
    );

    void render(
        SDL_GPUCommandBuffer* commandBuffer,
        SDL_GPURenderPass* renderPass,
        float aspectRatio
    );

    bool createCirclePipeline(
        SDL_GPUTextureFormat targetFormat
    );

    bool ensureCircleCapacity(
        std::size_t count
    );

private:
    SDL_GPUDevice* device_ = nullptr;

    SDL_GPUGraphicsPipeline*
        circlePipeline_ = nullptr;

    SDL_GPUBuffer*
        circleBuffer_ = nullptr;

    SDL_GPUTransferBuffer*
        circleTransferBuffer_ = nullptr;

    std::size_t circleCapacity_ = 0;

    std::vector<CircleInstance>
        circles_;

    Camera2D camera_;
};