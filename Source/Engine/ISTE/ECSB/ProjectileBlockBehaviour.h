#pragma once
#include "Behaviour.h"
#include "ISTE/Math/Vec3.h"

namespace ISTE
{
	class Scene;


	class ProjectileBlockBehaviour : public ISTE::Behaviour
	{
	public:
		void OnTrigger(EntityID aId) override;

	private:
	};
}