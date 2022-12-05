// covered in TimeHandler

//#pragma once
//#include <functional>
//
//namespace CU
//{
//	class StepTimer
//	{
//	public:
//		StepTimer() = default;
//		~StepTimer() = default;
//		
//		void Init(const size_t someUpdatesPerSecond, std::function<void()> aCallBack = nullptr);
//		const bool Update(const float aTimeDelta);
//
//	private:
//		std::function<void()> myCallBack = nullptr;
//		float myUpdateDelay = FLT_MAX;
//		float myElapsedTime = 0.f;
//	};
//}