#pragma once
#include "ISTE/Scene/Scene.h"
#include "AbstractComponentEdit.h"
namespace ISTE
{
	class EmitterComponentEdit : public AbstractComponentEdit
	{
	public:
		void Draw(ISTE::Scene* aScenePointer, Entity anEntity) override;
	private:
	};
}