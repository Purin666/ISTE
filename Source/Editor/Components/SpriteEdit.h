#pragma once
#include "ISTE/Scene/Scene.h"

#include "AbstractComponentEdit.h"
namespace ISTE
{
	class SpriteEdit : public AbstractComponentEdit
	{
	public:
		void Draw(ISTE::Scene* aScenePointer, Entity anEntity) override;
		void Sprite3DComponent(ISTE::Scene* aScenePointer, Entity anEntity);
		void Sprite2DComponent(ISTE::Scene* aScenePointer, Entity anEntity);
	private:

		CU::Vec3f myOriginalColour;
		CU::Vec2f myOriginalUV;
		CU::Vec2f myOriginalUVScale;
		CU::Vec2f myOriginalUVStart;
		CU::Vec2f myOriginalUVEnd;
		bool myIsEditing = false;
		bool myIsEditing2 = false;
		bool myIsEditing3 = false;
		bool myIsEditing4 = false;
		bool myIsEditing5 = false;

	};
}