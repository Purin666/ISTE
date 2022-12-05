#pragma once

#include "AudioClip.h"
#include "ISTE/ECSB/Behaviour.h"

#include <unordered_map>

#define ASP_DEFUALT				0x00
#define ASP_ADDITIVE			0x01
#define ASP_EXLUSIVE			0x02
#define ASP_IGNOREIFACTIVE		0x04
#define ASP_IGNOREIFPLAYING		0x08


namespace ISTE
{

	typedef unsigned short AudioSourcePlayFlag;

	class AudioHandler;

	//for now getting specific clip instances is not possible meaning that some undesired behaviour might arise ex: set pitch changing the pitch of all instances of that clip type
	class AudioSource : public Behaviour
	{
	public:
		AudioSource();
		~AudioSource();
		void Init() override;
		//aShouldStop = false could result in some undefined behaviour
		void Play(int aClip, AudioSourcePlayFlag aFlag = ASP_DEFUALT);
		void Pause(int aClip, bool aShouldPause);
		void PauseAll(bool aShouldPause);
		void Stop(int aClip);
		void StopAll();
		bool IsPlaying(int aClip);
		bool IsAnyPlaying();
		int PlayCount(int aClip);

		void Update(float) override;

		//not recommended to mix the load functions
		int LoadClip(std::string aPath, bool aShouldLoop = true, bool aActiveAfterDeath = false);
		bool LoadAndMapClip(int aMapValue, std::string aPath, bool aShouldLoop = true, bool aActiveAfterDeath = false);
		void UnLoadClip(int aClip);
		//inline int GetActiveClip() { return myActiveAudioClip; }
		//inline void SetActiveClip(int aIndex, bool aShouldOverride = true) { if (aShouldOverride) Stop(); myActiveAudioClip = aIndex; }
		void SetMinDistance(float aMinDistance);
		void SetMaxDistance(float aMaxDistance);

		void Use3DSpace(bool aUse3DSpace);

		void SetSourceVolume(float aVolume); 
		void IncrementSourceVolume(float aVolume);

		inline void SetSoundType(SoundTypes aSoundType) { mySoundType = aSoundType; }

		//these should be somthing applyed over everything not just the clip that is active
		void SetPitch(int aClip, float aPitch);// { myAudioClips[myActiveAudioClip].myChannel->setPitch(aPitch); }
		inline void SetPlayOnAwake(bool aPlayOnAwake) { myPlayOnAwake = aPlayOnAwake; }
		//only really used for playonawake //default by default is 0
		inline void SetDefaultClip(int aClip) { myActiveAudioClip = aClip; }

		void InfoDump(int aClip);

	private:
		friend class AudioSourceBehaviourCRAW;
		friend class AudioSourceBehaviourEdit;
		friend class AudioSourceSystem;

		FMOD_3D_ATTRIBUTES my3DAttributes;
		std::unordered_map<int, AudioClip> myAudioClips;
		AudioHandler* myAudioHandler = nullptr;
		float myVolume = 1.f;
		float myMinDistance = 0.1f;
		float myMaxDistance = 1000.f;
		int myTotalSounds = 0; 
		int myActiveAudioClip = 0;
		SoundTypes mySoundType = SoundTypes::eMaster;
		bool myIsPlaying = false;
		bool myPlayOnAwake = false;
		bool myIsUsing3DSpace = false;

	};
};