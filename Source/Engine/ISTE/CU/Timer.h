#pragma once
#include <chrono>
#include <ctime>
#include <ratio>

namespace CU
{
	using namespace std::chrono;
	class Timer
	{
	public:
		Timer();
		Timer(const Timer& aTimer) = delete;
		Timer& operator=(const Timer& aTimer) = delete;

		void Update();
		float GetDeltaTime() const;
		float GetTotalTime() const;
		__forceinline void ResetDeltaTime() { myDeltaTime = 0.f; }

	private:
		steady_clock::time_point myInitTime;
		steady_clock::time_point myLastTick;
		steady_clock::time_point myCurrentTime;

		float myTotalTime;
		float myDeltaTime; 
	};
}