#pragma once
#include "ISTE/Builder/ObjectBuilder.h"

namespace ISTE
{

	class DecalBuilder : public ObjectBuilder
	{ 
	public:
		~DecalBuilder();
		void Create(EntityID aId, GObject& aObject, Scene& aScene) override;

	};

};
