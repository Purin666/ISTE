#include "AudioSource.h"
#include "ISTE/Context.h"
#include "AudioHandler.h"

#include "ISTE/Scene/SceneHandler.h"
#include "ISTE/Scene/Scene.h"
#include "ISTE/Graphics/ComponentAndSystem/TransformComponent.h"

#include <filesystem>


#include "ISTE/CU/MemTrack.hpp"


namespace ISTE
{
	AudioSource::AudioSource()
	{
		myAudioHandler = Context::Get()->myAudioHandler;
	}

	AudioSource::~AudioSource()
	{
		for (auto& pair : myAudioClips)
		{
			for (auto& channel : pair.second.myActiveChannels)
			{
				if (pair.second.myPlayAfterDeath)
					continue;

				channel->stop();
			}
		}
	}

	void AudioSource::Init()
	{
		//myAudioHandler = Context::Get()->myAudioHandler;

		if (myPlayOnAwake)
		{
			Play(myActiveAudioClip);
		}
		

	}

	void AudioSource::Play(int aClip, AudioSourcePlayFlag aFlag)
	{

		//assert(myAudioClips.count(aClip) && "Clip was not loaded");

		if (myAudioClips.count(aClip) == 0)
			return;

		//myIsPlaying = true;


		if (aFlag & ASP_EXLUSIVE)
		{
			for (auto& [id, clip] : myAudioClips)
			{
				if (id == aClip)
					continue;

				for (auto& channel : clip.myActiveChannels)
				{
					channel->stop();
				}

				clip.myActiveChannels.clear();
			}
		}

		//If active and playing should probably be looked over since channels have a isplaying. 
		//However it seems to return true even if it's paused

		if (aFlag & ASP_IGNOREIFACTIVE)
		{
			if (myAudioClips[aClip].myActiveChannels.size() != 0)
				return;
		}

		if (aFlag & ASP_IGNOREIFPLAYING)
		{
			if (myAudioClips[aClip].myActiveChannels.size() != 0)
			{
				bool isPaused = false;

				for (auto& channel : myAudioClips[aClip].myActiveChannels)
				{
					channel->getPaused(&isPaused);

					if (!isPaused)
						return;
				}
			}
		}

		if (aFlag & ASP_ADDITIVE)
		{

			AudioClip& clip = myAudioClips.at(aClip);
			FMOD::Channel* nChannel;
			myAudioHandler->Play(clip.mySound, mySoundType, nChannel);
			nChannel->setVolume(myVolume);
			nChannel->set3DMinMaxDistance(myMinDistance, myMaxDistance);
			clip.myActiveChannels.push_back(nChannel);
			return;
		}



		AudioClip& clip = myAudioClips.at(aClip);
		for (auto& channel : clip.myActiveChannels)
		{
			channel->stop();
		}

		clip.myActiveChannels.clear();

		FMOD::Channel* channel;

		myAudioHandler->Play(clip.mySound, mySoundType, channel);
		channel->setVolume(myVolume);
		channel->set3DMinMaxDistance(myMinDistance, myMaxDistance);
		clip.myActiveChannels.push_back(channel);

	}
	void AudioSource::Pause(int aClip, bool aShouldPause)
	{
		for(auto& channel : myAudioClips[aClip].myActiveChannels)
			channel->setPaused(aShouldPause);
	}
	void AudioSource::PauseAll(bool aShouldPause)
	{
		for(auto& [id, clip] : myAudioClips)
			for (auto& channel : clip.myActiveChannels)
				channel->setPaused(aShouldPause);
	}
	void AudioSource::Stop(int aClip)
	{

		for (auto& channel : myAudioClips[aClip].myActiveChannels)
			channel->stop();

		myAudioClips[aClip].myActiveChannels.clear();

	}

	void AudioSource::StopAll()
	{
		for (auto& [id, clip] : myAudioClips)
			for (auto& channel : clip.myActiveChannels)
				channel->stop();
	}

	bool AudioSource::IsPlaying(int aClip)
	{
		for (auto& channel : myAudioClips[aClip].myActiveChannels)
		{
			bool check;
			channel->isPlaying(&check);
			if (check)
				return true;
		}

		return false;
	}

	bool AudioSource::IsAnyPlaying()
	{
		for (auto& [id, clip] : myAudioClips)
			for (auto& channel : clip.myActiveChannels)
			{
				bool check;
				channel->isPlaying(&check);
				if (check)
					return true;
			}

		return false;
	}

	int AudioSource::PlayCount(int aClip)
	{
		return (int)myAudioClips[aClip].myActiveChannels.size();
	}

	void AudioSource::Update(float)
	{
		for (auto& [id, clip] : myAudioClips)
		{
			bool isPlaying;
			for (int i = clip.myActiveChannels.size() - 1; i >= 0; i--)
			{
				clip.myActiveChannels[i]->isPlaying(&isPlaying);
				if (!isPlaying)
				{
					clip.myActiveChannels.erase(clip.myActiveChannels.begin() + i);
				}
			}
		}

		if (myIsUsing3DSpace)
		{
			TransformComponent* transform = Context::Get()->mySceneHandler->GetActiveScene().GetComponent<TransformComponent>(myHostId);

			if (transform == nullptr)
				return;

			my3DAttributes.position = { transform->myCachedTransform.GetTranslationV3().x, transform->myCachedTransform.GetTranslationV3().y, transform->myCachedTransform.GetTranslationV3().z };
			my3DAttributes.velocity = { 0,0,0 };

			for (auto& [id, clip] : myAudioClips)
				for (auto& channel : clip.myActiveChannels)
					channel->set3DAttributes(&my3DAttributes.position, &my3DAttributes.velocity);
		}
	}

	int AudioSource::LoadClip(std::string aPath, bool aShouldLoop, bool aActiveAfterDeath)
	{
		if (!std::filesystem::exists(aPath))
			return -1;

		UnLoadClip(myTotalSounds);

		myAudioClips.insert({ myTotalSounds, AudioClip() });
		AudioClip& clip = myAudioClips.at(myTotalSounds);
		clip.myPlayAfterDeath = aActiveAfterDeath;
		clip.myPath = aPath;
		clip.myLoop = aShouldLoop;
		myAudioHandler->LoadSound(clip, aShouldLoop, myIsUsing3DSpace);

		myTotalSounds++;

		return myTotalSounds - 1;
	}

	bool AudioSource::LoadAndMapClip(int aMapValue, std::string aPath, bool aShouldLoop, bool aActiveAfterDeath)
	{
		if (!std::filesystem::exists(aPath))
		{
			std::cout << "path: " << aPath << " Did not exist" << std::endl;
			return false;
		}

		UnLoadClip(aMapValue);

		myAudioClips.insert({ aMapValue, AudioClip() });
		AudioClip& clip = myAudioClips.at(aMapValue);
		clip.myPlayAfterDeath = aActiveAfterDeath;
		clip.myPath = aPath;
		clip.myLoop = aShouldLoop;
		myAudioHandler->LoadSound(clip, aShouldLoop, myIsUsing3DSpace);

		clip.myChannel->setVolume(myVolume);

		myTotalSounds++;

		return true;
	}

	void AudioSource::UnLoadClip(int aClip)
	{
		if (!myAudioClips.count(aClip))
			return;

		AudioClip& clip = myAudioClips.at(aClip);

		for (auto& channel : clip.myActiveChannels)
		{
			channel->stop();
		}

		clip.mySound->release();

		myAudioClips.erase(aClip);
	}

	void AudioSource::SetMinDistance(float aMinDistance)
	{
		myMinDistance = aMinDistance;

		for (auto& [id, clip] : myAudioClips)
		{
			for (auto& channel : clip.myActiveChannels)
			{
				channel->set3DMinMaxDistance(myMinDistance, myMaxDistance);
			}
		}
	}

	void AudioSource::SetMaxDistance(float aMaxDistance)
	{
		myMaxDistance = aMaxDistance;

		for (auto& [id, clip] : myAudioClips)
		{
			for (auto& channel : clip.myActiveChannels)
			{
				channel->set3DMinMaxDistance(myMinDistance, myMaxDistance);
			}
		}
	}

	void AudioSource::Use3DSpace(bool aUse3DSpace)
	{
		myIsUsing3DSpace = aUse3DSpace;

		for (auto& [id, clip] : myAudioClips)
		{
			for (auto& channel : clip.myActiveChannels)
			{
				FMOD_MODE mode;
				channel->getMode(&mode);

				if ((mode & FMOD_3D) != 0 && !aUse3DSpace)
				{
					mode ^= FMOD_3D;
					mode |= FMOD_2D;
				}

				if ((mode & FMOD_2D) != 0 && aUse3DSpace)
				{
					mode ^= FMOD_2D;
					mode |= FMOD_3D;
				}

				channel->setMode(mode);

			}
		}
	}

	void AudioSource::SetSourceVolume(float aVolume)
	{
		myVolume = aVolume;

		if (myVolume > 1)
			myVolume = 1;
		else if (myVolume < 0)
			myVolume = 0;

		for (auto& [id, clip] : myAudioClips)
			for (auto& channel : clip.myActiveChannels)
				channel->setVolume(myVolume);

	}

	void AudioSource::IncrementSourceVolume(float aVolume)
	{
		myVolume += aVolume;

		if (myVolume > 1)
			myVolume = 1;
		else if (myVolume < 0)
			myVolume = 0;

		for (auto& [id, clip] : myAudioClips)
			for (auto& channel : clip.myActiveChannels)
				channel->setVolume(myVolume);

	}
	void AudioSource::SetPitch(int aClip, float aPitch)
	{
		for (auto& channel : myAudioClips[aClip].myActiveChannels)
			channel->setPitch(aPitch);
	}
	void AudioSource::InfoDump(int aClip)
	{
		AudioClip& clip = myAudioClips.at(aClip);

		std::cout << "------------------------------General------------------------------" << std::endl;
		std::cout << "Should Loop: " << clip.myLoop << std::endl;
		std::cout << "Path: " << clip.myPath << std::endl;
		std::cout << "Should PlayAfterDeath: " << clip.myPlayAfterDeath << std::endl;
		std::cout << "Active Channels: " << clip.myActiveChannels.size() << std::endl;
		std::cout << "-------------------------------------------------------------------" << std::endl;

		std::cout << "------------------------------Sound------------------------------" << std::endl;

		std::cout << "-----------------------------------------------------------------" << std::endl;

		for (auto& channel : clip.myActiveChannels)
		{
			std::cout << "------------------------------Clip------------------------------" << std::endl;

			std::cout << "----------------------------------------------------------------" << std::endl;
		}

	}
}