#pragma once

#include "ISTE/Scene/Scene.h"
#include "AbstractComponentEdit.h"

namespace ISTE
{
	class SpotLightEdit : public AbstractComponentEdit
	{
	public:
		void Draw(ISTE::Scene* aScenePointer, Entity anEntity) override;
		void SpotLightCompEdit(ISTE::Scene* aScenePointer, Entity anEntity);
		void ShadowCastingSpotLightCompEdit(ISTE::Scene* aScenePointer, Entity anEntity);

	private:
		CU::Vec4f myOriginalColour;
		float myOGInnerAngle;
		float myOGOuterAngle;
		float myOGRange;
		bool myOGShadowCasting;

		bool myIsEditing = false;
		bool myIsEditing1 = false;
		bool myIsEditing2 = false;
		bool myIsEditing3 = false;
		bool myIsEditing4 = false;

	};
}