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

    bool initialize(SDL_Window *window);
    void shutdown();

    bool beginFrame(Color clearColor);
    void endFrame();

    void setCamera(const Camera &camera);

    void circle(
        Vec2 center,
        float radius,
        Color color);

    void segment(
        Vec2 start,
        Vec2 end,
        float thickness,
        Color color);

    void rect(
        Vec2 center,
        Vec2 size,
        float rotation,
        Color color);

private:
    enum class ShapeType : uint32_t
    {
        Circle,
        Segment,
        Rect
    };

    struct ShapeInstance
    {
        Vec2 center;
        Vec2 halfSize;

        float rotation;
        uint32_t type;

        Color color;
    };

private:
    SDL_Window *window_ = nullptr;
    SDL_GPUDevice *device_ = nullptr;

    SDL_GPUCommandBuffer *commandBuffer_ = nullptr;
    SDL_GPUTexture *swapchainTexture_ = nullptr;

    SDL_GPUBuffer *quadBuffer_ = nullptr;

    SDL_GPUBuffer *shapeBuffer_ = nullptr;
    SDL_GPUTransferBuffer *shapeTransferBuffer_ = nullptr;

    SDL_GPUGraphicsPipeline *shapePipeline_ = nullptr;

    std::size_t shapeCapacity_ = 1024;

    Color clearColor_ = Color::black();

    Camera camera_;

    std::vector<ShapeInstance> shapes_;
    SDL_GPUTextureFormat targetFormat_{};

    bool createBuffers();
    bool uploadQuad();
    bool uploadShapes();
    bool createPipeline();

    SDL_GPUShader *loadShader(
        const char *path,
        SDL_GPUShaderStage stage,
        Uint32 uniformBuffers);
};