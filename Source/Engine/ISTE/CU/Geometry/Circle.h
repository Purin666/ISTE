#pragma once
#include "ISTE/Math/Vec2.h"

namespace CU
{
    template <class T>
    class Circle
    {
    public:
        T radius;
        Vec2<T> center;

        Circle() = default;
        Circle(const Circle<T>& aCircle) = default;
        Circle(const Vec2<T>& aCenter, T& aRadius) : center(aCenter), radius(aRadius) {}

        void InitWithCenterAndRadius(const Vec2<T>& aCenter, T& aRadius) { center = aCenter; radius = aRadius; }

        bool IsInside(const Vec2<T>& aPosition) const
        {
            return (Vec2<T>(aPosition - center).LengthSqr() <= radius * radius);
        }
    };
}