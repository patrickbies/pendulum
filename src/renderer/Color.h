#pragma once

struct Color
{
    float r;
    float g;
    float b;
    float a;

    static constexpr Color black()
    {
        return {0.0f, 0.0f, 0.0f, 1.0f};
    }

    static constexpr Color white()
    {
        return {1.0f, 1.0f, 1.0f, 1.0f};
    }

    static constexpr Color red()
    {
        return {1.0f, 0.0f, 0.0f, 1.0f};
    }
};