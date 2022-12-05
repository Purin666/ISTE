#pragma once


#include "AbstractComponentEdit.h"

#include <string>

namespace ISTE
{
	class AudioSourceBehaviourEdit : public AbstractComponentEdit
	{
	public:
		~AudioSourceBehaviourEdit() = default;
		void Draw(ISTE::Scene* aScenePointer, Entity anEntity) override;
	private:
		std::string myClipEditPath = "";
		std::string myClipPath = "";

		std::string myErrorText = "";
		float myErrorTime = 3;
		float myTimer = myErrorTime;
		float myStartFadeTime = 1;
	};
}
