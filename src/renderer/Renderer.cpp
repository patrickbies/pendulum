#include "renderer/Renderer.h"

#include <iostream>
#include <cmath>
#include <cstring>
#include <fstream>
#include <string>
#include <cstddef>

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

    targetFormat_ =
        SDL_GetGPUSwapchainTextureFormat(
            device_,
            window_);

    if (!createBuffers())
    {
        shutdown();
        return false;
    }

    if (!uploadQuad())
    {
        shutdown();
        return false;
    }

    if (!createPipeline())
    {
        shutdown();
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

SDL_GPUShader *Renderer::loadShader(
    const char *filename,
    SDL_GPUShaderStage stage,
    Uint32 uniformBuffers)
{
    std::string path =
        std::string(SDL_GetBasePath()) + "shaders/" + filename;

    std::ifstream file(
        path,
        std::ios::binary);

    if (!file)
    {
        std::cerr
            << "Failed to open shader: "
            << path
            << '\n';

        return nullptr;
    }

    std::string source(
        std::istreambuf_iterator<char>(file),
        {});

    SDL_GPUShaderCreateInfo info{};

    info.code =
        reinterpret_cast<const Uint8 *>(
            source.c_str());

    info.code_size =
        source.size() + 1;

    info.entrypoint =
        stage == SDL_GPU_SHADERSTAGE_VERTEX
            ? "vertexMain"
            : "fragmentMain";

    info.format =
        SDL_GPU_SHADERFORMAT_MSL;

    info.stage = stage;

    info.num_samplers = 0;
    info.num_storage_textures = 0;
    info.num_storage_buffers = 0;
    info.num_uniform_buffers =
        uniformBuffers;

    SDL_GPUShader *shader =
        SDL_CreateGPUShader(
            device_,
            &info);

    if (!shader)
    {
        std::cerr
            << "Failed to create shader "
            << filename
            << ": "
            << SDL_GetError()
            << '\n';
    }

    return shader;
}

bool Renderer::createPipeline()
{
    static_assert(sizeof(Vec2) == 8);
    static_assert(sizeof(Color) == 16);
    static_assert(sizeof(ShapeInstance) == 40);

    SDL_GPUShader *vertexShader =
        loadShader(
            "shape.vert.metal",
            SDL_GPU_SHADERSTAGE_VERTEX,
            1);

    if (!vertexShader)
        return false;

    SDL_GPUShader *fragmentShader =
        loadShader(
            "shape.frag.metal",
            SDL_GPU_SHADERSTAGE_FRAGMENT,
            0);

    if (!fragmentShader)
    {
        SDL_ReleaseGPUShader(
            device_,
            vertexShader);

        return false;
    }

    SDL_GPUVertexBufferDescription buffers[2]{};

    buffers[0] = {
        .slot = 0,
        .pitch = sizeof(Vec2),
        .input_rate =
            SDL_GPU_VERTEXINPUTRATE_VERTEX,
        .instance_step_rate = 0};

    buffers[1] = {
        .slot = 1,
        .pitch = sizeof(ShapeInstance),
        .input_rate =
            SDL_GPU_VERTEXINPUTRATE_INSTANCE,
        .instance_step_rate = 0};

    SDL_GPUVertexAttribute attributes[6]{};

    // quad local position
    attributes[0] = {
        .location = 0,
        .buffer_slot = 0,
        .format =
            SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2,
        .offset = 0};

    // center
    attributes[1] = {
        .location = 1,
        .buffer_slot = 1,
        .format =
            SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2,
        .offset =
            offsetof(
                ShapeInstance,
                center)};

    // halfSize
    attributes[2] = {
        .location = 2,
        .buffer_slot = 1,
        .format =
            SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2,
        .offset =
            offsetof(
                ShapeInstance,
                halfSize)};

    // rotation
    attributes[3] = {
        .location = 3,
        .buffer_slot = 1,
        .format =
            SDL_GPU_VERTEXELEMENTFORMAT_FLOAT,
        .offset =
            offsetof(
                ShapeInstance,
                rotation)};

    // shape type
    attributes[4] = {
        .location = 4,
        .buffer_slot = 1,
        .format =
            SDL_GPU_VERTEXELEMENTFORMAT_UINT,
        .offset =
            offsetof(
                ShapeInstance,
                type)};

    // color
    attributes[5] = {
        .location = 5,
        .buffer_slot = 1,
        .format =
            SDL_GPU_VERTEXELEMENTFORMAT_FLOAT4,
        .offset =
            offsetof(
                ShapeInstance,
                color)};

    SDL_GPUColorTargetDescription target{};

    target.format =
        targetFormat_;

    target.blend_state.enable_blend =
        true;

    target.blend_state.src_color_blendfactor =
        SDL_GPU_BLENDFACTOR_SRC_ALPHA;

    target.blend_state.dst_color_blendfactor =
        SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;

    target.blend_state.color_blend_op =
        SDL_GPU_BLENDOP_ADD;

    target.blend_state.src_alpha_blendfactor =
        SDL_GPU_BLENDFACTOR_ONE;

    target.blend_state.dst_alpha_blendfactor =
        SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;

    target.blend_state.alpha_blend_op =
        SDL_GPU_BLENDOP_ADD;

    SDL_GPUGraphicsPipelineCreateInfo info{};

    info.vertex_shader =
        vertexShader;

    info.fragment_shader =
        fragmentShader;

    info.vertex_input_state = {
        .vertex_buffer_descriptions =
            buffers,

        .num_vertex_buffers = 2,

        .vertex_attributes =
            attributes,

        .num_vertex_attributes = 6};

    info.primitive_type =
        SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;

    info.target_info = {
        .color_target_descriptions =
            &target,

        .num_color_targets = 1,

        .has_depth_stencil_target =
            false};

    shapePipeline_ =
        SDL_CreateGPUGraphicsPipeline(
            device_,
            &info);

    SDL_ReleaseGPUShader(
        device_,
        vertexShader);

    SDL_ReleaseGPUShader(
        device_,
        fragmentShader);

    if (!shapePipeline_)
    {
        std::cerr
            << "Failed to create graphics pipeline: "
            << SDL_GetError()
            << '\n';

        return false;
    }

    return true;
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

bool Renderer::createBuffers()
{
    SDL_GPUBufferCreateInfo quadInfo{};
    quadInfo.usage = SDL_GPU_BUFFERUSAGE_VERTEX;
    quadInfo.size = sizeof(Vec2) * 6;

    quadBuffer_ =
        SDL_CreateGPUBuffer(
            device_,
            &quadInfo);

    if (!quadBuffer_)
    {
        std::cerr
            << "Failed to create quad buffer: "
            << SDL_GetError()
            << '\n';

        return false;
    }

    // One ShapeInstance per rendered shape.
    SDL_GPUBufferCreateInfo shapeInfo{};

    shapeInfo.usage =
        SDL_GPU_BUFFERUSAGE_VERTEX;

    shapeInfo.size =
        static_cast<Uint32>(
            shapeCapacity_ * sizeof(ShapeInstance));

    shapeBuffer_ =
        SDL_CreateGPUBuffer(
            device_,
            &shapeInfo);

    if (!shapeBuffer_)
    {
        std::cerr
            << "Failed to create shape buffer: "
            << SDL_GetError()
            << '\n';

        return false;
    }

    // CPU-visible staging buffer used to upload
    // ShapeInstance data every frame.
    SDL_GPUTransferBufferCreateInfo transferInfo{};

    transferInfo.usage =
        SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;

    transferInfo.size =
        shapeInfo.size;

    shapeTransferBuffer_ =
        SDL_CreateGPUTransferBuffer(
            device_,
            &transferInfo);

    if (!shapeTransferBuffer_)
    {
        std::cerr
            << "Failed to create shape transfer buffer: "
            << SDL_GetError()
            << '\n';

        return false;
    }

    return true;
}

bool Renderer::uploadQuad()
{
    const Vec2 quadVertices[6] = {
        {-1.0f, -1.0f},
        {1.0f, -1.0f},
        {1.0f, 1.0f},

        {-1.0f, -1.0f},
        {1.0f, 1.0f},
        {-1.0f, 1.0f}};

    SDL_GPUTransferBufferCreateInfo transferInfo{};

    transferInfo.usage =
        SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;

    transferInfo.size =
        sizeof(quadVertices);

    SDL_GPUTransferBuffer *transferBuffer =
        SDL_CreateGPUTransferBuffer(
            device_,
            &transferInfo);

    if (!transferBuffer)
    {
        std::cerr
            << "Failed to create quad transfer buffer: "
            << SDL_GetError()
            << '\n';

        return false;
    }

    void *data =
        SDL_MapGPUTransferBuffer(
            device_,
            transferBuffer,
            false);

    if (!data)
    {
        SDL_ReleaseGPUTransferBuffer(
            device_,
            transferBuffer);

        return false;
    }

    std::memcpy(
        data,
        quadVertices,
        sizeof(quadVertices));

    SDL_UnmapGPUTransferBuffer(
        device_,
        transferBuffer);

    SDL_GPUCommandBuffer *commandBuffer =
        SDL_AcquireGPUCommandBuffer(device_);

    if (!commandBuffer)
    {
        SDL_ReleaseGPUTransferBuffer(
            device_,
            transferBuffer);

        return false;
    }

    SDL_GPUCopyPass *copyPass =
        SDL_BeginGPUCopyPass(commandBuffer);

    SDL_GPUTransferBufferLocation source{
        .transfer_buffer = transferBuffer,
        .offset = 0};

    SDL_GPUBufferRegion destination{
        .buffer = quadBuffer_,
        .offset = 0,
        .size = sizeof(quadVertices)};

    SDL_UploadToGPUBuffer(
        copyPass,
        &source,
        &destination,
        false);

    SDL_EndGPUCopyPass(copyPass);

    const bool success =
        SDL_SubmitGPUCommandBuffer(
            commandBuffer);

    SDL_ReleaseGPUTransferBuffer(
        device_,
        transferBuffer);

    if (!success)
    {
        std::cerr
            << "Failed to upload quad: "
            << SDL_GetError()
            << '\n';
    }

    return success;
}

bool Renderer::uploadShapes()
{
    if (shapes_.empty())
        return true;

    if (shapes_.size() > shapeCapacity_)
    {
        std::cerr
            << "Shape capacity exceeded\n";

        return false;
    }

    const Uint32 size =
        static_cast<Uint32>(
            shapes_.size() * sizeof(ShapeInstance));

    void *data =
        SDL_MapGPUTransferBuffer(
            device_,
            shapeTransferBuffer_,
            true);

    if (!data)
    {
        std::cerr
            << "Failed to map shape transfer buffer: "
            << SDL_GetError()
            << '\n';

        return false;
    }

    std::memcpy(
        data,
        shapes_.data(),
        size);

    SDL_UnmapGPUTransferBuffer(
        device_,
        shapeTransferBuffer_);

    SDL_GPUCopyPass *copyPass =
        SDL_BeginGPUCopyPass(
            commandBuffer_);

    SDL_GPUTransferBufferLocation source{
        .transfer_buffer =
            shapeTransferBuffer_,
        .offset = 0};

    SDL_GPUBufferRegion destination{
        .buffer = shapeBuffer_,
        .offset = 0,
        .size = size};

    SDL_UploadToGPUBuffer(
        copyPass,
        &source,
        &destination,
        true);

    SDL_EndGPUCopyPass(copyPass);

    return true;
}

void Renderer::endFrame()
{
    if (!commandBuffer_ || !swapchainTexture_)
        return;

    if (!uploadShapes())
    {
        SDL_CancelGPUCommandBuffer(
            commandBuffer_);

        commandBuffer_ = nullptr;
        swapchainTexture_ = nullptr;

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

    if (shapePipeline_)
    {
        SDL_ReleaseGPUGraphicsPipeline(
            device_,
            shapePipeline_);

        shapePipeline_ = nullptr;
    }

    if (shapeTransferBuffer_)
    {
        SDL_ReleaseGPUTransferBuffer(
            device_,
            shapeTransferBuffer_);

        shapeTransferBuffer_ = nullptr;
    }

    if (shapeBuffer_)
    {
        SDL_ReleaseGPUBuffer(
            device_,
            shapeBuffer_);

        shapeBuffer_ = nullptr;
    }

    if (quadBuffer_)
    {
        SDL_ReleaseGPUBuffer(
            device_,
            quadBuffer_);

        quadBuffer_ = nullptr;
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