#pragma once
#include "AudioClip.h"
#include <vector>
#include <unordered_map>
#include "ISTE/Math/Matrix4x4.h"

namespace ISTE
{
	enum class PreLoadedSounds
	{
		eMuisc_Menu,
		eMusic_Boss,
		eMusic_Catacombs,
		eMusic_OldTown,
		eUI_ButtonClick,
		eUI_StartGame,
		eCount
	};

	enum class Banks
	{
		eMusic,
		eCount,
	};

	enum class EventTypes
	{
		eMusic,
		eCount,
	};

	struct SoundContext
	{
		FMOD::System* coreSystem = NULL;
		FMOD::Studio::System* system = NULL;
	};

	class AudioHandler
	{
	public:
		AudioHandler();
		~AudioHandler();

		//studio
		FMOD::Studio::EventInstance* PlayEvent(const FMOD_GUID aEventID, const bool aPlayOnce = false);
		FMOD::Studio::EventInstance* PlayEvent(const FMOD_GUID anID, const FMOD_3D_ATTRIBUTES& a3Dattrib, const bool aPlayOnce = false);
		void ChangeGlobalEventVolume(EventTypes aType, float aVolume);
		void IncrementGlobalEventVolume(EventTypes aType, float aVolume);
		float GetGlobalEventVolume(EventTypes aType);

		//core
		void Play(FMOD::Sound*& aSound, SoundTypes aSoundType, FMOD::Channel*& outChannel);
		void PauseChannel(SoundTypes aSoundType, bool aShouldPause);
		void LoadSound(AudioClip& aClip, bool aShouldLoop, bool aUse3D);
		void ChangeGlobalVolume(SoundTypes aType, float aVolume);
		void IncrementGlobalVolume(SoundTypes aType, float aVolume);
		float GetGlobalVolume(SoundTypes aType);

		//shared
		void Update();
		void SetStaticListener(const CU::Matrix4x4f& aMatrix);
		void HookListener(const CU::Matrix4x4f& aHookMatrix);
		void UnHookListener();

		//Self
		void PlayPreLoadedSound(PreLoadedSounds aSound);
		void StopPreLoadedSound(PreLoadedSounds aSound);
		void StopAllPreLoadedSounds();

	private:
		FMOD_3D_ATTRIBUTES myListener;
		CU::Matrix4x4f const* myListenerTransform = nullptr;
		SoundContext mySoundContext;
		std::unordered_map<SoundTypes, FMOD::ChannelGroup*> mySoundGroups;

		//events
		std::unordered_map<EventTypes, FMOD::Studio::Bus*> myChannels;
		std::unordered_map<Banks, FMOD::Studio::Bank*> myBanks;

		struct SelfAudioClip
		{
			AudioClip myClip;
			SoundTypes myType;
		};

		std::unordered_map<PreLoadedSounds, SelfAudioClip> myAudioClips;

	};
}