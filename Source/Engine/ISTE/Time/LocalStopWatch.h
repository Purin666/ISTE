#pragma once
#include "ISTE/Time/LocalTimer.h"

namespace ISTE
{
    class LocalStopWatch : public LocalTimer
    {
    public:
        void Update(const float aTimeDelta) override;

        inline void SetInvokeTime(const float anInvokeTime) { myInvokeTime = anInvokeTime; }
        inline void InvokeAndRestart() { myCallback(); myElapsedTime = 0.f; }

    private:
        float myElapsedTime = 0.f;
        float myInvokeTime = FLT_MAX;
    };
}