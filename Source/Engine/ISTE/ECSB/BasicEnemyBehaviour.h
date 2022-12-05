#pragma once
#include "Behaviour.h"
#include "ISTE/Math/Vec3.h"

namespace ISTE
{
	class Scene;


	class BasicEnemyBehaviour : public ISTE::Behaviour
	{
	public:
		void Init() override;
		void Update(float aDeltaTime) override;

	private:
		float mySightRange = 20;
		float mySpeed = 2;
		CU::Vec3<float> myTargetPosition;
		Scene* myActiveScene = nullptr;
	};
}