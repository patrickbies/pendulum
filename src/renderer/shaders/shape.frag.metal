#include <metal_stdlib>
using namespace metal;

struct FragmentInput
{
    float4 position [[position]];

    float2 localPosition [[user(locn0)]];
    float2 halfSize      [[user(locn1)]];
    uint type            [[user(locn2), flat]];
    float4 color         [[user(locn3)]];
};

float circleSDF(
    float2 p,
    float radius
)
{
    return length(p) - radius;
}

float rectSDF(
    float2 p,
    float2 halfSize
)
{
    float2 q =
        abs(p) - halfSize;

    return
        length(max(q, float2(0.0)))
        +
        min(
            max(q.x, q.y),
            0.0
        );
}

float capsuleSDF(
    float2 p,
    float halfLength,
    float radius
)
{
    p.x -= clamp(
        p.x,
        -halfLength,
        halfLength
    );

    return length(p) - radius;
}

fragment float4 fragmentMain(
    FragmentInput input [[stage_in]]
)
{
    float distance;

    switch (input.type)
    {
        // Circle
        case 0:
            distance =
                circleSDF(
                    input.localPosition,
                    input.halfSize.x
                );
            break;

        // Segment
        case 1:
        {
            const float radius =
                input.halfSize.y;

            const float halfLength =
                input.halfSize.x - radius;

            distance =
                capsuleSDF(
                    input.localPosition,
                    halfLength,
                    radius
                );

            break;
        }

        // Rect
        default:
            distance =
                rectSDF(
                    input.localPosition,
                    input.halfSize
                );
            break;
    }

    const float aa =
        fwidth(distance);

    const float alpha =
        1.0 -
        smoothstep(
            -aa,
            0.0,
            distance
        );

    return float4(
        input.color.rgb,
        input.color.a * alpha
    );
}