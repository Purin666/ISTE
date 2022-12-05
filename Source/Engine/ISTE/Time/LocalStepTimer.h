#pragma once
#include "ISTE/Time/LocalTimer.h"

namespace ISTE
{
    class LocalStepTimer : public LocalTimer
    {
    public:
        void Update(const float aTimeDelta) override;

        inline void SetDelay(const float someSeconds) { myDelay = someSeconds; }
        inline void SetDelay(const int someTicksPerSecond) { myDelay = 1.f / (float)someTicksPerSecond; }
        inline void InvokeAndRestart() { myCallback(); myElapsedTime = 0.f; }

    private:
        float myElapsedTime = 0.f;
        float myDelay = FLT_MAX;
    };
}