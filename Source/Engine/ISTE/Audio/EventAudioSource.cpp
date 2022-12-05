#include "EventAudioSource.h"
#include "AudioHandler.h"

#include "ISTE/Context.h"
#include "ISTE/Scene/SceneHandler.h"
#include "ISTE/Scene/Scene.h"
#include "ISTE/Graphics/ComponentAndSystem/TransformComponent.h"

#include "ISTE/CU/MemTrack.hpp"

namespace ISTE
{
	EventAudioSource::EventAudioSource()
		: myEventInstance(nullptr)
	{}

	EventAudioSource::~EventAudioSource()
	{
		Stop();
	}

	void EventAudioSource::Update(float)
	{
		if (myEventInstance == nullptr)
			return;

		TransformComponent* transform = Context::Get()->mySceneHandler->GetActiveScene().GetComponent<TransformComponent>(myHostId);

		CU::Vec3f forward, up;
		forward = transform->myCachedTransform.GetForward().GetNormalized();
		up = transform->myCachedTransform.GetUp().GetNormalized();

		my3DAttributes.position = { transform->myCachedTransform.GetTranslationV3().x, transform->myCachedTransform.GetTranslationV3().y, transform->myCachedTransform.GetTranslationV3().z };
		my3DAttributes.forward = { forward.x, forward.y, forward.z };
		my3DAttributes.up = { up.x, up.y, up.z };

		myEventInstance->set3DAttributes(&my3DAttributes);

		
	}

	void EventAudioSource::PlayEvent(FMOD_GUID aEvent, bool aStopPrevious)
	{
		if (aStopPrevious && myEventInstance)
		{
			myEventInstance->stop(FMOD_STUDIO_STOP_ALLOWFADEOUT);
		}

		myEventInstance = Context::Get()->myAudioHandler->PlayEvent(aEvent, my3DAttributes);
	}

	void EventAudioSource::SetParameter(const char* aName, const float aValue)
	{
		if (!myEventInstance)
		{
			return;
		}
		myEventInstance->setParameterByName(aName, aValue);
	}

	void EventAudioSource::Stop()
	{
		if (!myEventInstance)
		{
			return;
		}
		myEventInstance->stop(FMOD_STUDIO_STOP_ALLOWFADEOUT);
	}

	void EventAudioSource::Pause()
	{
		if (!myEventInstance)
		{
			return;
		}
		myEventInstance->setPaused(true);
	}

	void EventAudioSource::Resume()
	{
		if (!myEventInstance)
		{
			return;
		}
		myEventInstance->setPaused(false);
	}

	void EventAudioSource::SetVolume(float aVolume)
	{
		if (!myEventInstance)
		{
			return;
		}
		myEventInstance->setVolume(aVolume);
	}
}