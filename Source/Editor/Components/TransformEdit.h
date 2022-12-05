#pragma once
#include "ISTE/Scene/Scene.h"
#include "ISTE/Math/Vec3.h"
#include <ISTE/Math/Euler.h>
#include "AbstractComponentEdit.h"
namespace ISTE
{
	class TransformEdit : public AbstractComponentEdit
	{
	public:
		void Draw(ISTE::Scene* aScenePointer, Entity anEntity) override;
	private:
		CU::Vec3f myOriginPos;
		CU::Vec3f myOriginSize;
		CU::Euler myOriginRot = {};

		bool myIsEditing = false;
		bool myIsEditing2 = false;
		bool myIsEditing3 = false;
	};
}