#pragma once
#include "ISTE/Time/LocalTimer.h"

namespace ISTE
{
    class LocalCountDown : public LocalTimer
    {
    public:
        void Update(const float aTimeDelta) override;

        inline void SetDuration(const float aDuration) { myDuration = aDuration; myRemainingTime = aDuration; }
        inline void SetRepeat(const float aFlag) { myRepeatFlag = aFlag; }
        inline void InvokeAndRestart() { myCallback(); myRemainingTime = myDuration; }

    private:
        float myRemainingTime = FLT_MAX;
        float myDuration = FLT_MAX;
        bool myRepeatFlag = false;
    };
}