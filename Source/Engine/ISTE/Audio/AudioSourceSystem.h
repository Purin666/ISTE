#pragma once

#include "ISTE/ECSB/System.h"
#include "ISTE/Audio/AudioClip.h"

#include "ISTE/Graphics/RenderDefines.h"

namespace ISTE
{

	class AudioSourceSystem : public System
	{
	public:

		void Init();

		void StopAll();
		void PauseAll(bool aShouldPause);
		void StopAll(SoundTypes aType);
		void PauseAll(SoundTypes aType, bool aShouldPause);

		void PrepareAudioSourceIcons();

	private:
		TextureID myAudioSourceID;
		
	};
}
