#pragma once
#include "ISTE/Scene/Scene.h"
#include "AbstractComponentEdit.h"
#include "ISTE/Graphics/ComponentAndSystem/ModelComponent.h"
namespace ISTE
{
	class ModelEdit : public AbstractComponentEdit
	{
	public:
		void Draw(ISTE::Scene* aScenePointer, Entity anEntity);

	private:
		void DragDrop(ISTE::ModelComponent* aModelComponent);
		CU::Vec3f myOriginalColour;
		CU::Vec2f myOriginalUV;
		CU::Vec2f myOriginalUVScale;
		bool myIsEditing = false;
		bool myIsEditing2 = false;
		bool myIsEditing3 = false;
		bool myIsEditing4 = false;
	};
}