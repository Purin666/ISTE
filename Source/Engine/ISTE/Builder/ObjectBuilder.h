#pragma once

#include "../ECSB/ECSDefines.hpp"
#include "BuilderDataStructs.h"

namespace ISTE
{
	class Scene;

	class ObjectBuilder
	{
	public:
		virtual void Create(EntityID aid, GObject& aObject, Scene& aScene) = 0;
		virtual ~ObjectBuilder() = default;

	private:

	};
}