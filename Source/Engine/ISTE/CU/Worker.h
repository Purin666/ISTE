#pragma once

#include <functional>
#include <vector>
#include <thread>

#include <mutex>

namespace CU
{
	template<typename T>
	struct WorkerContainer
	{
		size_t myId = 0;
		T myData;
	};


	//this has a lot of uneccesary mutexes //if anything should use atomic
	template<typename T>
	class Worker
	{
	public:
		void AddWroker(std::function<T()> aWorker)
		{
			myWorkers.push_back(aWorker);
		}
		void RemoveWorker(size_t aWorkerIndex)
		{
			myWorkers.erase(myWorkers.begin() + aWorkerIndex);
		}

		void Run()
		{
			myIsRunning = true;
			for (size_t i = 0; i < myWorkers.size(); i++)
			{
				myThreads.push_back(std::thread([this, i]() { CallFunc(i); }));
			}

			for (size_t i = 0; i < myThreads.size(); i++)
			{
				myThreads[i].join();
			}
		}

		void RunAsync()
		{
			myIsRunning = true;
			for (size_t i = 0; i < myWorkers.size(); i++)
			{
				myThreads.push_back(std::thread([this, i]() { CallFunc(i); }));
			}
		}

		void WaitForThreads()
		{
			for (size_t i = 0; i < myThreads.size(); i++)
			{
				myThreads[i].join();
			}
		}

		bool IsRunning() { return myIsRunning; }

		std::vector<WorkerContainer<T>>& GetContainer() { return myContainer; }

	private:
		void CallFunc(size_t index)
		{
			myCallGuard.lock();
			auto func = myWorkers[index];
			myCallGuard.unlock();

			WorkerContainer<T> container;

			container.myData = func();

			container.myId = index;

			AddToContainer(container);
			myWorkerCount++;

			if (myWorkerCount == myWorkers.size())
			{
				myIsRunning = false;
				myWorkerCount = 0;
			}

		}

		void AddToContainer(WorkerContainer<T> con)
		{
			myContainerGuard.lock();
			myContainer.push_back(con);
			myContainerGuard.unlock();
		}

		std::mutex myCallGuard;
		std::mutex myContainerGuard;
		std::vector<std::thread> myThreads;
		std::vector<std::function<T()>> myWorkers;
		std::vector<WorkerContainer<T>> myContainer;

		std::atomic_bool myIsRunning = false;
		std::atomic_int myWorkerCount = 0;

	};
}