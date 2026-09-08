#include "renderer/renderer2d/Renderer2D.h"

#include <cstring>

bool Renderer2D::initialize(
    SDL_GPUDevice* device,
    SDL_GPUTextureFormat targetFormat
)
{
    device_ = device;

    circleCapacity_ = 1024;

    SDL_GPUBufferCreateInfo bufferInfo{};

    bufferInfo.usage =
        SDL_GPU_BUFFERUSAGE_GRAPHICS_STORAGE_READ;

    bufferInfo.size =
        static_cast<Uint32>(
            circleCapacity_ *
            sizeof(CircleInstance)
        );

    circleBuffer_ =
        SDL_CreateGPUBuffer(
            device_,
            &bufferInfo
        );

    if (!circleBuffer_)
    {
        return false;
    }

    SDL_GPUTransferBufferCreateInfo
        transferInfo{};

    transferInfo.usage =
        SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;

    transferInfo.size =
        bufferInfo.size;

    circleTransferBuffer_ =
        SDL_CreateGPUTransferBuffer(
            device_,
            &transferInfo
        );

    if (!circleTransferBuffer_)
    {
        return false;
    }

    if (!createCirclePipeline(
            targetFormat
        ))
    {
        return false;
    }

    return true;
}

void Renderer2D::upload(
    SDL_GPUCommandBuffer* commandBuffer
)
{
    if (circles_.empty())
        return;

    ensureCircleCapacity(
        circles_.size()
    );

    const std::size_t bytes =
        circles_.size() *
        sizeof(CircleInstance);

    void* mapped =
        SDL_MapGPUTransferBuffer(
            device_,
            circleTransferBuffer_,
            true
        );

    if (!mapped)
    {
        return;
    }

    std::memcpy(
        mapped,
        circles_.data(),
        bytes
    );

    SDL_UnmapGPUTransferBuffer(
        device_,
        circleTransferBuffer_
    );

    SDL_GPUCopyPass* copyPass =
        SDL_BeginGPUCopyPass(
            commandBuffer
        );

    SDL_GPUTransferBufferLocation source{
        .transfer_buffer =
            circleTransferBuffer_,
        .offset = 0
    };

    SDL_GPUBufferRegion destination{
        .buffer = circleBuffer_,
        .offset = 0,
        .size =
            static_cast<Uint32>(bytes)
    };

    SDL_UploadToGPUBuffer(
        copyPass,
        &source,
        &destination,
        true
    );

    SDL_EndGPUCopyPass(
        copyPass
    );
}