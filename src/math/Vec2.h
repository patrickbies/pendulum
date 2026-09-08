#pragma once

template<typename T>
struct Vec2
{
    T x = 0;
    T y = 0;

    constexpr Vec2() = default;
    constexpr Vec2(T x, T y) : x(x), y(y) {}

    constexpr Vec2 operator+(const Vec2& v) const
    {
        return {x + v.x, y + v.y};
    }

    constexpr Vec2 operator-(const Vec2& v) const
    {
        return {x - v.x, y - v.y};
    }

    constexpr Vec2 operator-() const
    {
        return {-x, -y};
    }

    constexpr Vec2 operator*(T s) const
    {
        return {x * s, y * s};
    }

    constexpr Vec2 operator/(T s) const
    {
        return {x / s, y / s};
    }

    constexpr Vec2& operator+=(const Vec2& v)
    {
        x += v.x;
        y += v.y;
        return *this;
    }

    constexpr Vec2& operator-=(const Vec2& v)
    {
        x -= v.x;
        y -= v.y;
        return *this;
    }

    constexpr Vec2& operator*=(T s)
    {
        x *= s;
        y *= s;
        return *this;
    }

    constexpr Vec2& operator/=(T s)
    {
        x /= s;
        y /= s;
        return *this;
    }
};

using Vec2f = Vec2<float>;
using Vec2d = Vec2<double>; 

template<typename T>
constexpr Vec2<T> operator*(T s, const Vec2<T>& v)
{
    return v * s;
}

template<typename T>
constexpr T dot(const Vec2<T>& a, const Vec2<T>& b)
{
    return a.x * b.x + a.y * b.y;
}

template<typename T>
constexpr T cross(const Vec2<T>& a, const Vec2<T>& b)
{
    return a.x * b.y - a.y * b.x;
}

template<typename T>
constexpr Vec2<T> cross(T s, const Vec2<T>& v)
{
    return {-s * v.y, s * v.x};
}

template<typename T>
constexpr Vec2<T> cross(const Vec2<T>& v, T s)
{
    return {s * v.y, -s * v.x};
}

template<typename T>
constexpr T lengthSquared(const Vec2<T>& v)
{
    return dot(v, v);
}

template<typename T>
T length(const Vec2<T>& v)
{
    return std::sqrt(lengthSquared(v));
}

template<typename T>
Vec2<T> normalized(const Vec2<T>& v)
{
    const T len = length(v);

    if (len <= std::numeric_limits<T>::epsilon())
        return {};

    return v / len;
}

template<typename T>
constexpr Vec2<T> perpendicular(const Vec2<T>& v)
{
    return {-v.y, v.x};
}