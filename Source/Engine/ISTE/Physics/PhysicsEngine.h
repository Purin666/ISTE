#pragma once


namespace ISTE
{
	class TriggerSystem;

	class PhysicsEngine
	{
	public:
		void Init();
		void Update();

	private:
		TriggerSystem* myTriggerSystem = nullptr;
	};
}