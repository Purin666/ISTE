#include "StopWatch.h"

using namespace std::chrono;

#include "ISTE/CU/MemTrack.hpp"

namespace CU
{

	StopWatch::StopWatch()
	{
	}

	void StopWatch::Start()
	{
		myIsActive = true;
		myIsReset = false;
		myStartPoint = steady_clock::now();
	}

	void StopWatch::Stop()
	{
		if (myIsActive)
		{
			steady_clock::time_point now = steady_clock::now();
			switch (myMessure)
			{
			case CU::SW_Messures::Nano:
				duration<double, std::nano> d = now - myStartPoint;
				myTotalTime += d.count();
				break;
			case CU::SW_Messures::Micro:
				duration<double, std::micro> du = now - myStartPoint;
				myTotalTime += du.count();
				break;
			case CU::SW_Messures::Mili:
				duration<double, std::milli> dur = now - myStartPoint;
				myTotalTime += dur.count();
				break;
			default:
				break;
			}

			myIsActive = false;
		}
	}

	void StopWatch::Reset()
	{
		myTotalTime = 0;
		myIsActive = false;
		myIsReset = true;
	}

	double StopWatch::GetTime()
	{
		if (myIsActive)
		{
			steady_clock::time_point now = steady_clock::now();
			switch (myMessure)
			{
			case CU::SW_Messures::Nano:
				duration<double, std::nano> d = now - myStartPoint;
				myTotalTime += d.count();
				break;
			case CU::SW_Messures::Micro:
				duration<double, std::micro> du = now - myStartPoint;
				myTotalTime += du.count();
				break;
			case CU::SW_Messures::Mili:
				duration<double, std::milli> dur = now - myStartPoint;
				myTotalTime += dur.count();
				break;
			default:
				break;
			}


			myStartPoint = now;
		}

		return myTotalTime;
	}

	void StopWatch::SetMessure(SW_Messures aMessure)
	{
		if (myIsReset)
		{
			myMessure = aMessure;
		}
	}

	//void StopWatch::SetInterval(std::function<void> aCallback, float aInterval)
	//{
	//}


}