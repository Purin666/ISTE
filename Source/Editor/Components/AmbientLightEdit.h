#pragma once
#include "ISTE/Scene/Scene.h"
#include "AbstractComponentEdit.h"
namespace ISTE
{
	class AmbientLightComponent;
	class AmbientLightEdit : public AbstractComponentEdit
	{
	public:
		void Draw(ISTE::Scene* aScenePointer, Entity anEntity) override;
	private:
		void DragDrop(ISTE::AmbientLightComponent* anAmbientComponent);
		bool myIsEditing = false;
		bool myIsEditing2 = false;
		bool myIsEditing3 = false;
		CU::Vec4f	myOriginGroundColorIntensity;
		CU::Vec4f	myOriginSkyColorIntensity;
		float		myOriginalCubeMapIntensity;
	};
}
