#pragma once
#include "ISTE/Scene/Scene.h"
#include "AbstractComponentEdit.h"
#include "ISTE/Math/Vec3.h"

namespace ISTE{
	class DecalEdit : public AbstractComponentEdit {
	public:
		DecalEdit();
		~DecalEdit();
		void Draw(ISTE::Scene* aScenePointer, Entity anEntity) override;
	private:
		CU::Vec3f	myLerpValues;
		float		myThreshold;
		float		myCoverageFlag;
		bool okmfse = false;
	};
}