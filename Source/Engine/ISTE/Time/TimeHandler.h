#pragma once
#include <string>

namespace CU
{
	class Timer;
}
namespace ISTE
{
	struct CountDown;
	struct StopWatch;
	struct StepTimer;
	struct TimerPool;
	struct UpdateTimer;
	class Engine;

	class TimeHandler
	{
	public:
		const float GetDeltaTime() const;
		const float GetTotalTime() const;
		const float GetTime(const std::string& aName);
		void ResetDeltaTime();
		__forceinline void SetTimeScale(const float aScale) { myTimeScale = aScale; }

		void AddTimer(const CountDown& aTimer);
		void AddTimer(const StopWatch& aTimer);
		void AddTimer(const StepTimer& aTimer);
		void AddTimer(const UpdateTimer& aTimer);

		void RemoveTimer(const std::string& aName);
		void RemoveAllTimers();

		void SetOn(const std::string& aName, bool aFlag);
		
		void PauseAllActiveTimers();	// Paused all active, ticking timers
		void ResumeAllPausedTimers();	// Resumes all timers paused when PauseAllActiveTimers() was last called

		void TickTimers(); // TODO:: Should be private, but loop is in main() //Mathias
		void ProcessTimers();

		void InvokeTimer(const std::string& aName);
		void InvokeAllTimers();

	private:
		friend class ISTE::Engine;
		TimerPool* myPool;
		CU::Timer* myCUTimer;
		float myTimeScale = 1.f;
	};
}