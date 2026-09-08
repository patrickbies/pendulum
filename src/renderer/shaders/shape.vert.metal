#include <metal_stdlib>
using namespace metal;

struct VertexInput
{
    float2 localPosition [[attribute(0)]];

    float2 center        [[attribute(1)]];
    float2 halfSize      [[attribute(2)]];
    float rotation       [[attribute(3)]];
    uint   type          [[attribute(4)]];
    float4 color         [[attribute(5)]];
};

struct CameraUniform
{
    float2 center;
    float2 halfExtents;
};

struct VertexOutput
{
    float4 position [[position]];

    float2 localPosition [[user(locn0)]];
    float2 halfSize      [[user(locn1)]];
    uint type            [[user(locn2), flat]];
    float4 color         [[user(locn3)]];
};

vertex VertexOutput vertexMain(
    VertexInput input [[stage_in]],
    constant CameraUniform& camera [[buffer(0)]]
)
{
    const float2 local =
        input.localPosition * input.halfSize;

    const float c = cos(input.rotation);
    const float s = sin(input.rotation);

    const float2 rotated = {
        c * local.x - s * local.y,
        s * local.x + c * local.y
    };

    const float2 world =
        input.center + rotated;

    const float2 clip =
        (world - camera.center)
        / camera.halfExtents;

    VertexOutput output;

    output.position = float4(
        clip,
        0.0,
        1.0
    );

    output.localPosition = local;
    output.halfSize = input.halfSize;
    output.type = input.type;
    output.color = input.color;

    return output;
}