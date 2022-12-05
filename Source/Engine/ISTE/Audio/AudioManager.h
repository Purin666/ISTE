//AUTHOR Oskar Schramm 2k22

#pragma once
#include "FMOD/fmod_studio.hpp"
#include "FMOD/fmod_studio_guids.hpp"
#include <vector>
#include <unordered_map>
#include "ISTE/Math/Matrix4x4.h"
#include "EventAudioSource.h"

namespace ISTE
{
	enum class Channels
	{
		Master,
		SFX,
		Music,
		UI
	};

	struct SoundContext
	{
		FMOD::System* coreSystem = NULL;
		FMOD::Studio::System* system = NULL;
	};

	class AudioManager
	{
	public:
		AudioManager();
		~AudioManager();

		void Init();
		void Update();

		void SetListenerTransform(const CU::Matrix4x4f& aTransform);
		void SetListenerTransform(const CU::Matrix4x4f* const aTransform);

		FMOD::Studio::EventInstance* PlayEvent(const FMOD_GUID aEventID, const bool aPlayOnce = false);
		FMOD::Studio::EventInstance* PlayEvent(const FMOD_GUID anID, const FMOD_3D_ATTRIBUTES& a3Dattrib, const bool aPlayOnce = false);
		void StopEvent(const FMOD_GUID anID);
		void StopAllEvents();
		void PauseAllEvents();
		void StartAllPausedEvents();
		void SetLevelMusic(int aLevel);
		FMOD::Studio::EventInstance* GetLevelMusic();
		float GetVolume(const Channels aChannelID) const;
		void SetChannelVolume(const Channels aChannelID, float aVolume);
		void SetVolume(float aVolume);
		void MuteChannel(const Channels aChannelID, bool aValue = true);
		void UnmuteChannel(const Channels aChannelID);
		void MuteAll();

		inline bool GetFriday() { return myIsFriday; }

		EventAudioSource* CreateAudioClip();
		void RemoveAudioClip(EventAudioSource* aAudioClip);
	private:
		//AudioManager();
		//~AudioManager();
		AudioManager(const AudioManager&) = delete;
		AudioManager operator=(const AudioManager&) = delete;
	private:
		FMOD_3D_ATTRIBUTES myListenerAttributes;
		CU::Matrix4x4f const* myListenerTransform;
		FMOD::Studio::EventInstance* myLevelMusic;

		SoundContext myContext;
		bool myIsFriday = false;
		std::vector<EventAudioSource> myAudioClips3D;

		std::unordered_map<Channels, FMOD::Studio::Bus*> myChannels;
		FMOD::Studio::Bank* myMasterBank = nullptr;
		FMOD::Studio::Bank* myMusicBank = nullptr;
		FMOD::Studio::Bank* myPlayerBank = nullptr;
		FMOD::Studio::Bank* myUIBank = nullptr;
		FMOD::Studio::Bank* myIntroBank = nullptr;
		FMOD::Studio::Bank* myEnvironmentBank = nullptr;
	};
}