#pragma once

#include "Components/AbstractComponentEdit.h"
#include "ISTE/Scene/Scene.h"
namespace ISTE
{
	class Inspector
	{
	public:
		~Inspector();
		void Init();
		void SetTarget(const Entity& anEntity);
		void Update();
		void ResetTarget();
	private:
		//Going to have to move components to their own classes later.
		void CustomShaderComponent(ISTE::Scene* aScenePointer);

		Entity myTarget;
		std::vector<AbstractComponentEdit*> myComponentsToEdit;
		bool myInspecting = false;

		std::string myTmpEntityName;
		std::string myTmpEntityTag;
		std::string myTmpEntityLayer;

		bool myDisplayComponents = false;
		bool myDisplayRemoveComponents = false;

	};
}