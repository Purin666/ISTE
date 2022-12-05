#pragma once

#include <chrono>
#include <functional>

namespace CU
{
	enum class SW_Messures
	{
		Nano,
		Micro,
		Mili,
	};

	class StopWatch
	{
	public:
		StopWatch();
		void Start();
		void Stop();
		void Reset();
		double GetTime();
		void SetMessure(SW_Messures aMessure);
		//void SetInterval(std::function<void> aCallback, float aInterval);

	private:
		//std::function<void> aCallback = nullptr;
		//float aInterval = 0;

		//timer

		std::chrono::steady_clock::time_point myStartPoint;
		bool myIsActive = false;
		bool myIsReset = true;
		double myTotalTime = 0;

		SW_Messures myMessure = SW_Messures::Mili;

		//

	};

}