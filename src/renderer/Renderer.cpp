#include "Renderer.h"

#include <iostream>

bool Renderer::initialize(SDL_Window* window)
{
    window_ = window;

    device_ = SDL_CreateGPUDevice(
        SDL_GPU_SHADERFORMAT_MSL,
        true,
        nullptr
    );

    if (!device_)
        return false;

    if (!SDL_ClaimWindowForGPUDevice(
            device_,
            window_
        ))
    {
        return false;
    }

    const auto format =
        SDL_GetGPUSwapchainTextureFormat(
            device_,
            window_
        );

    if (!renderer2D_.initialize(
            device_,
            format
        ))
    {
        return false;
    }

    return true;
}

bool Renderer::beginFrame(
    const FrameConfig& config
)
{
    frameConfig_ = config;

    commandBuffer_ =
        SDL_AcquireGPUCommandBuffer(
            device_
        );

    if (!commandBuffer_)
    {
        return false;
    }

    swapchainTexture_ = nullptr;

    if (!SDL_WaitAndAcquireGPUSwapchainTexture(
            commandBuffer_,
            window_,
            &swapchainTexture_,
            nullptr,
            nullptr
        ))
    {
        SDL_CancelGPUCommandBuffer(
            commandBuffer_
        );

        commandBuffer_ = nullptr;

        return false;
    }

    if (!swapchainTexture_)
    {
        SDL_SubmitGPUCommandBuffer(
            commandBuffer_
        );

        commandBuffer_ = nullptr;

        return false;
    }

    // Clears the CPU-side shape queues.
    renderer2D_.beginFrame();

    return true;
}

void Renderer::endFrame()
{
    SDL_EndGPURenderPass(
        renderPass_
    );

    renderPass_ = nullptr;

    if (!SDL_SubmitGPUCommandBuffer(
            commandBuffer_
        ))
    {
        std::cerr
            << "Failed to submit GPU command buffer: "
            << SDL_GetError()
            << '\n';
    }

    commandBuffer_ = nullptr;
    swapchainTexture_ = nullptr;
}

void Renderer::shutdown()
{
    if (!device_)
        return;

    SDL_WaitForGPUIdle(device_);

    if (window_)
    {
        SDL_ReleaseWindowFromGPUDevice(
            device_,
            window_
        );
    }

    SDL_DestroyGPUDevice(device_);

    device_ = nullptr;
    window_ = nullptr;
}

Renderer::~Renderer()
{
    shutdown();
}