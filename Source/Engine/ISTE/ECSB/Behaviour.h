#pragma once

#include "ECSDefines.hpp"
#include "../CU/Database.h"
namespace ISTE
{
	class Behaviour
	{
	public:
		virtual ~Behaviour() = default;
		virtual void Init() {};
		virtual void Update(float) {};
		virtual void OnTrigger(EntityID) {};
		virtual void UpdateWithDatabase(CU::Database<true>& aBehaviourDatabase) {};

	private:
		friend class Scene;
		ComponentID myComponentId;

	protected:
		EntityID myHostId = INVALID_ENTITY;
	};
}