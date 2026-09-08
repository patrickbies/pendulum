#include "renderer/Renderer.h"

#include <iostream>

bool Renderer::initialize(SDL_Window *window)
{
    window_ = window;

    device_ = SDL_CreateGPUDevice(
        SDL_GPU_SHADERFORMAT_MSL,
        true,
        nullptr);

    if (!device_)
    {
        std::cerr
            << "Failed to create GPU device: "
            << SDL_GetError()
            << '\n';

        return false;
    }

    if (!SDL_ClaimWindowForGPUDevice(
            device_,
            window_))
    {
        std::cerr
            << "Failed to claim window for GPU device: "
            << SDL_GetError()
            << '\n';

        SDL_DestroyGPUDevice(device_);
        device_ = nullptr;

        return false;
    }

    std::cout
        << "GPU backend: "
        << SDL_GetGPUDeviceDriver(device_)
        << '\n';

    return true;
}

bool Renderer::beginFrame(Color clearColor)
{
    clearColor_ = clearColor;

    // Anything submitted last frame is gone.
    shapes_.clear();

    commandBuffer_ =
        SDL_AcquireGPUCommandBuffer(device_);

    if (!commandBuffer_)
    {
        std::cerr
            << "Failed to acquire GPU command buffer: "
            << SDL_GetError()
            << '\n';

        return false;
    }

    swapchainTexture_ = nullptr;

    if (!SDL_WaitAndAcquireGPUSwapchainTexture(
            commandBuffer_,
            window_,
            &swapchainTexture_,
            nullptr,
            nullptr))
    {
        std::cerr
            << "Failed to acquire swapchain texture: "
            << SDL_GetError()
            << '\n';

        SDL_CancelGPUCommandBuffer(commandBuffer_);

        commandBuffer_ = nullptr;

        return false;
    }

    // This can happen while the window is minimized.
    if (!swapchainTexture_)
    {
        SDL_SubmitGPUCommandBuffer(commandBuffer_);

        commandBuffer_ = nullptr;

        return false;
    }

    return true;
}

void Renderer::setCamera(const Camera &camera)
{
    camera_ = camera;
}

void Renderer::circle(
    Vec2 center,
    float radius,
    Color color)
{
    if (radius <= 0.0f)
        return;

    shapes_.push_back({.center = center,
                       .halfSize = {radius, radius},
                       .rotation = 0.0f,
                       .type = static_cast<uint32_t>(ShapeType::Circle),
                       .color = color});
}

void Renderer::segment(
    Vec2 start,
    Vec2 end,
    float thickness,
    Color color)
{
    if (thickness <= 0.0f)
        return;

    const float dx = end.x - start.x;
    const float dy = end.y - start.y;

    const float length =
        std::sqrt(dx * dx + dy * dy);

    if (length == 0.0f)
        return;

    const float radius =
        thickness * 0.5f;

    shapes_.push_back({.center = {
                           (start.x + end.x) * 0.5f,
                           (start.y + end.y) * 0.5f},

                       .halfSize = {length * 0.5f + radius, radius},

                       .rotation = std::atan2(dy, dx),

                       .type = static_cast<uint32_t>(ShapeType::Segment),

                       .color = color});
}

void Renderer::rect(
    Vec2 center,
    Vec2 size,
    float rotation,
    Color color)
{
    if (size.x <= 0.0f || size.y <= 0.0f)
        return;

    shapes_.push_back({.center = center,
                       .halfSize = {
                           size.x * 0.5f,
                           size.y * 0.5f},
                       .rotation = rotation,
                       .type = static_cast<uint32_t>(ShapeType::Rect),
                       .color = color});
}

void Renderer::endFrame()
{
    if (!commandBuffer_ || !swapchainTexture_)
    {
        return;
    }

    SDL_GPUColorTargetInfo colorTarget{};

    colorTarget.texture =
        swapchainTexture_;

    colorTarget.clear_color = {
        clearColor_.r,
        clearColor_.g,
        clearColor_.b,
        clearColor_.a};

    colorTarget.load_op =
        SDL_GPU_LOADOP_CLEAR;

    colorTarget.store_op =
        SDL_GPU_STOREOP_STORE;

    SDL_GPURenderPass *renderPass =
        SDL_BeginGPURenderPass(
            commandBuffer_,
            &colorTarget,
            1,
            nullptr);

    if (!renderPass)
    {
        std::cerr
            << "Failed to begin GPU render pass: "
            << SDL_GetError()
            << '\n';

        SDL_CancelGPUCommandBuffer(commandBuffer_);

        commandBuffer_ = nullptr;
        swapchainTexture_ = nullptr;

        return;
    }

    // rendering will go here

    SDL_EndGPURenderPass(renderPass);

    if (!SDL_SubmitGPUCommandBuffer(commandBuffer_))
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
    {
        return;
    }

    SDL_WaitForGPUIdle(device_);

    if (window_)
    {
        SDL_ReleaseWindowFromGPUDevice(
            device_,
            window_);
    }

    SDL_DestroyGPUDevice(device_);

    device_ = nullptr;
    window_ = nullptr;
}

Renderer::~Renderer()
{
    shutdown();
}