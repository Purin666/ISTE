#pragma once
#include "ISTE/Scene/Scene.h"
#include "AbstractComponentEdit.h"
namespace ISTE
{
	class CustomShaderEdit : public AbstractComponentEdit
	{
	public:
		void Draw(ISTE::Scene* aScenePointer, Entity anEntity) override;
	private:

	};
}