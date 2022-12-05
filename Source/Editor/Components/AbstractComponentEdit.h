#pragma once
#include "ISTE/Scene/Scene.h"
namespace ISTE
{
	class AbstractComponentEdit
	{
	public:
		virtual ~AbstractComponentEdit() = default;
		virtual void Draw(ISTE::Scene* /*aScenePointer*/, Entity /*anEntity*/) { return; }
	};
}