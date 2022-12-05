#pragma once
#include <Vector>

#include "ISTE/ECSB/System.h"
#include "ISTE/Graphics/ComponentAndSystem/TransformComponent.h"

namespace ISTE
{
	struct Context;
	class Scene;
	class ComponentPool;

	class TransformSystem : public System
	{
	public:
		TransformSystem();
		~TransformSystem() {};

		void Init();

		void EntityParentedCallback(EntityID aChild);
		void EntityUnParentedCallback(EntityID aChild);

		void UpdateToSpace(TransformComponent* aParent, TransformComponent* aChild, EntityID aChildId);
		void UpdateFromSpace(TransformComponent* aParent, TransformComponent* aChild, EntityID aChildId);
		void UpdateTransform();
		void CheckTransform(TransformComponent* aTransform, EntityID aIndex, Scene& aScene, ComponentPool& aPool);
		void CheckCache(EntityID aIndex, Scene& aScene, ComponentPool& aPool);
		void CachecTransform(TransformComponent* aTransform, EntityID aIndex);



	private:
		Context* myCtx = nullptr;
		std::vector<TransformComponent> myLastFrameTransforms;

	};
}