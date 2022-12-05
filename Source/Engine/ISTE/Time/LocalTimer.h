#pragma once
#include <functional>

namespace ISTE
{
    class LocalTimer
    {
    public:
        virtual ~LocalTimer() = default;

        virtual void Update(const float aTimeDelta) = 0;

        inline void SetCallback(std::function<void()> aCallback) { myCallback = aCallback; }
        inline void SetOn(const bool aFlag) { myOnFlag = aFlag; }

    protected:
        std::function<void()> myCallback = nullptr;
        bool myOnFlag = true;
    };
}