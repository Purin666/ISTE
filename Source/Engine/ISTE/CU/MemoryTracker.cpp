#include "MemoryTracker.h"

#include <windows.h>
#include <mutex>
#include <unordered_map>
#include <debugapi.h>

#define nNoMansLandSize 4

typedef struct _CrtMemBlockHeader
{
	struct _CrtMemBlockHeader* pBlockHeaderNext;
	struct _CrtMemBlockHeader* pBlockHeaderPrev;
	char* szFileName;
	int                         nLine;
#ifdef _WIN64
	/* These items are reversed on Win64 to eliminate gaps in the struct
	 * and ensure that sizeof(struct)%16 == 0, so 16-byte alignment is
	 * maintained in the debug heap.
	 */
	int                         nBlockUse;
	size_t                      nDataSize;
#else  /* _WIN64 */
	size_t                      nDataSize;
	int                         nBlockUse;
#endif  /* _WIN64 */
	long                        lRequest;
	unsigned char               gap[nNoMansLandSize];
	/* followed by:
	 *  unsigned char           data[nDataSize];
	 *  unsigned char           anotherGap[nNoMansLandSize];
	 */
} _CrtMemBlockHeader;

uint64_t totalAllocationSize;

struct Mem
{
	_CrtMemBlockHeader* myHeader = nullptr;
	size_t size = 0;
};

static std::unordered_map<long, size_t> allocations;
static bool isAllocating = false;
static std::mutex locAllocationMapMutex;

std::atomic<bool> lock = false;

void Lock() { while (lock.exchange(true, std::memory_order_acquire)); }

void Unlock() { lock.store(false, std::memory_order_release); }

int AllocHook(int allocType, void* userData, size_t size, int blockType, long requestNumber, const unsigned char* filename, int lineNumber)
{
	if (blockType == _CRT_BLOCK || isAllocating)
		return true;

	isAllocating = true;

	Lock();

	//std::lock_guard<std::mutex> guard(locAllocationMapMutex);

	bool alloc = false;
	bool free = false;
	switch (allocType)
	{
	case _HOOK_ALLOC:
	{
		alloc = true;
		break;
	}
	case _HOOK_REALLOC:
	{
		alloc = true;
		free = true;
		break;
	}
	case _HOOK_FREE:
	{
		free = true;
		break;
	}
	}

	if (alloc)
	{
		totalAllocationSize += size;
		allocations.insert({requestNumber, size });
	}

	if (free)
	{
		// read request number from header
		size_t headerSize = sizeof(_CrtMemBlockHeader);
		_CrtMemBlockHeader* pHead;
		size_t ptr = (size_t)userData - headerSize;
		pHead = (_CrtMemBlockHeader*)(ptr);

		long freeRequestNumber = pHead->lRequest;

		totalAllocationSize -= allocations[freeRequestNumber];
		allocations.erase(freeRequestNumber);
	}

	Unlock();

	isAllocating = false;
	return true;
	
}

namespace CU
{

	void MemoryTracker::StartTracking(bool aShouldHook)
	{
		if (aShouldHook)
		{
			_CrtSetAllocHook(&AllocHook);
		}

		int tmpFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);

		// Turn on leak-checking bit.
		tmpFlag |= _CRTDBG_LEAK_CHECK_DF;

		// Turn off CRT block checking bit.
		tmpFlag &= ~_CRTDBG_CHECK_CRT_DF;

		// Set flag to the new value.
		_CrtSetDbgFlag(tmpFlag);
	}

	void MemoryTracker::StopTracking()
	{
		_CrtSetAllocHook(nullptr);
	}

	void MemoryTracker::StopAndPrintAllocationData()
	{
		_CrtSetAllocHook(nullptr);

		for (auto& pair : allocations)
		{
			char b[100];
			sprintf_s(b, "== Allocated object size: %d\n", pair.second);
			OutputDebugStringA(b);
		}

		OutputDebugStringA("================================================================================\n");
		char buffer[100];
		sprintf_s(buffer, "== Remaining Allocation Size: %d\n", totalAllocationSize);
		OutputDebugStringA(buffer);
		char buffera[100];
		sprintf_s(buffera, "== Remaining Allocated Objects: %d\n", allocations.size());
		OutputDebugStringA(buffera);
		OutputDebugStringA("================================================================================\n");

		allocations.clear();
	}

	uint64_t MemoryTracker::GetTotalAllocationCount()
	{
		return totalAllocationSize;
	}
	uint64_t MemoryTracker::GetTotalAllocatedOjbects()
	{
		return allocations.size();
	}
}