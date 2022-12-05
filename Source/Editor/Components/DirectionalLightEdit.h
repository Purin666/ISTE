#pragma once

#include "ISTE/Scene/Scene.h"
#include "AbstractComponentEdit.h" 

namespace ISTE
{
	class DirectionalLightEdit : public AbstractComponentEdit
	{
	public:
		void Draw(ISTE::Scene* aScenePointer, Entity anEntity) override;

	private:
		CU::Vec4f myOriginalColour;

		bool myIsEditing = false;
	};
}