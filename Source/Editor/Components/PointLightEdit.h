#pragma once
#include "ISTE/Scene/Scene.h"
#include "AbstractComponentEdit.h"
namespace ISTE
{
	class PointLightEdit : public AbstractComponentEdit
	{
	public:
		void Draw(ISTE::Scene* aScenePointer, Entity anEntity) override;
		void PointLightCompEdit(ISTE::Scene* aScenePointer, Entity anEntity);
		void ShadowCastingPointLightCompEdit(ISTE::Scene* aScenePointer, Entity anEntity);

	private:
		CU::Vec4f myOriginalColour;
		float myOGRadius;
		bool myOGShadowCasting;

		bool myIsEditing = false;
		bool myIsEditing1 = false;
		bool myIsEditing2 = false;
	};
}