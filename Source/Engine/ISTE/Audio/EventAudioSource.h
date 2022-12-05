#pragma once
#include "ISTE/Math/Matrix4x4.h"
#include "FMOD/fmod_common.h"
#include "FMOD/fmod_studio.hpp"
#include "ISTE/ECSB/Behaviour.h"

struct FMOD_3D_ATTRIBUTES;

namespace ISTE
{

	class EventAudioSource : public Behaviour
	{
	public:
		EventAudioSource();
		~EventAudioSource();

		void Update(float) override;
		void PlayEvent(FMOD_GUID aEvent, bool aStopPrevious = true);
		void SetParameter(const char* aName, const float value);

		void Stop();
		void Pause();
		void Resume();
		void SetVolume(float aVolume);

	private:
		FMOD::Studio::EventInstance* myEventInstance;
		FMOD_3D_ATTRIBUTES my3DAttributes;
	};
}