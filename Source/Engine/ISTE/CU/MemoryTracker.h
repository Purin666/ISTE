#pragma once

#include <new>
namespace CU
{
	class MemoryTracker
	{
	public:
		void StartTracking(bool aShouldHook);
		void StopTracking();
		void StopAndPrintAllocationData();
		static uint64_t GetTotalAllocationCount();
		static uint64_t GetTotalAllocatedOjbects();

	private:

	};
}