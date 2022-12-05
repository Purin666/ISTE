#pragma once
#include <functional>

#include <string>

namespace ISTE
{
	enum class TimerType { eCountDown, eStopWatch, eStepTimer, eUpdateTimer };
	struct CountDown
	{
		std::function<void()> callback = nullptr;
		std::string name = "Untitled CountDown";
		bool repeat = false;
		float duration = FLT_MAX;

		float remainingTime = FLT_MAX;
	};
	struct StopWatch
	{
		std::function<void()> callback = nullptr;
		std::string name = "Untitled StopWatch";
		float invokeTime = FLT_MAX;

		float elapsedTime = 0.f;
	};
	struct StepTimer
	{
		std::function<void()> callback = nullptr;
		std::string name = "Untitled StepTimer";
		float delay = FLT_MAX;

		float elapsedTime = 0.f;
	};
	struct UpdateTimer
	{
		std::function<void()> callback = nullptr;
		std::string name = "Untitled StepTimer";
		unsigned int framesToSkip = 0;
		float duration = FLT_MAX;

		unsigned int skipped = 0;
		float elapsedTime = 0.f;
	};


	__forceinline const float TicksPerSecond(const size_t someTicks)
	{
		return 1.f / someTicks;
	}
}
