#pragma once
#include "ISTE/Time/LocalTimer.h"

namespace ISTE
{
    class LocalUpdateTimer : public LocalTimer
    {
    public:
        void Update(const float aTimeDelta) override;

        inline void SetDuration(const float aDuration) { myDuration = aDuration; }
        inline void SetFramesToSkip(const size_t anAmount) { myFramesToSkip = anAmount; }
        inline void InvokeAndRestart() { myCallback(); myElapsedTime = 0.f; mySkippedFrames = 0; }

    private:
        float myElapsedTime = 0.f;
        float myDuration = FLT_MAX;
        size_t mySkippedFrames = 0;
        size_t myFramesToSkip = 0;
    };
}