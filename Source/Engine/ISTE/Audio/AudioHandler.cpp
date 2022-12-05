#include "AudioHandler.h"

#include "ISTE/CU/MemTrack.hpp"

namespace ISTE
{
	AudioHandler::AudioHandler()
	{
		FMOD::Studio::System::create(&mySoundContext.system);
		FMOD::System_Create(&mySoundContext.coreSystem);

		mySoundContext.system->getCoreSystem(&mySoundContext.coreSystem);

		mySoundContext.system->initialize(512, FMOD_INIT_NORMAL, 0, nullptr);
		mySoundContext.coreSystem->init(512, FMOD_INIT_NORMAL, 0);

		//set up preloadedaudio

		//music
		myAudioClips.insert({ PreLoadedSounds::eMuisc_Menu, SelfAudioClip() });
		myAudioClips.insert({ PreLoadedSounds::eMusic_Boss, SelfAudioClip() });
		myAudioClips.insert({ PreLoadedSounds::eMusic_Catacombs, SelfAudioClip() });
		myAudioClips.insert({ PreLoadedSounds::eMusic_OldTown, SelfAudioClip() });

		//UI
		myAudioClips.insert({ PreLoadedSounds::eUI_ButtonClick, SelfAudioClip() });
		myAudioClips.insert({ PreLoadedSounds::eUI_StartGame, SelfAudioClip() });

		FMOD_MODE mode = FMOD_DEFAULT;
		mode |= FMOD_2D;
		mode |= FMOD_LOOP_NORMAL;

		FMOD_MODE modeNoLoop = FMOD_DEFAULT;
		mode |= FMOD_2D;

		SelfAudioClip&  sWTTBPClip = myAudioClips.at(PreLoadedSounds::eMuisc_Menu);
		sWTTBPClip.myType = SoundTypes::eMusic;
		mySoundContext.coreSystem->createSound("../Assets/Audio/Sounds/Music_Menu+boss.wav", mode, 0, &sWTTBPClip.myClip.mySound);

		SelfAudioClip& sBossClip = myAudioClips.at(PreLoadedSounds::eMusic_Boss);
		sBossClip.myType = SoundTypes::eMusic;
		mySoundContext.coreSystem->createSound("../Assets/Audio/Sounds/Music_NewBoss.mp3", mode, 0, &sBossClip.myClip.mySound);

		SelfAudioClip& sLevel1Clip = myAudioClips.at(PreLoadedSounds::eMusic_Catacombs);
		sLevel1Clip.myType = SoundTypes::eMusic;
		mySoundContext.coreSystem->createSound("../Assets/Audio/Sounds/Music_Catacombs.wav", mode, 0, &sLevel1Clip.myClip.mySound);

		SelfAudioClip& sLevel2Clip = myAudioClips.at(PreLoadedSounds::eMusic_OldTown);
		sLevel2Clip.myType = SoundTypes::eMusic;
		mySoundContext.coreSystem->createSound("../Assets/Audio/Sounds/Music_OldTown.wav", mode, 0, &sLevel2Clip.myClip.mySound);

		SelfAudioClip& sUI_ButtonClip = myAudioClips.at(PreLoadedSounds::eUI_ButtonClick);
		sUI_ButtonClip.myType = SoundTypes::eUI;
		mySoundContext.coreSystem->createSound("../Assets/Audio/Sounds/UI_Click.wav", modeNoLoop, 0, &sUI_ButtonClip.myClip.mySound);

		SelfAudioClip& sUI_StartGame = myAudioClips.at(PreLoadedSounds::eUI_StartGame);
		sUI_ButtonClip.myType = SoundTypes::eUI;
		mySoundContext.coreSystem->createSound("../Assets/Audio/Sounds/UI_StartGame.wav", modeNoLoop, 0, &sUI_StartGame.myClip.mySound);



		//

		//set up non-studio
		FMOD::ChannelGroup* master;
		mySoundContext.coreSystem->createChannelGroup("Master", &master);
		FMOD::ChannelGroup* music;
		mySoundContext.coreSystem->createChannelGroup("Music", &music);
		FMOD::ChannelGroup* sfx;
		mySoundContext.coreSystem->createChannelGroup("SFX", &sfx);
		FMOD::ChannelGroup* ui;
		mySoundContext.coreSystem->createChannelGroup("UI", &ui);
		FMOD::ChannelGroup* ambiance;
		mySoundContext.coreSystem->createChannelGroup("Ambiance", &ambiance);

		mySoundGroups.insert({ SoundTypes::eMaster, master });
		mySoundGroups.insert({ SoundTypes::eMusic, music });
		mySoundGroups.insert({ SoundTypes::eSFX, sfx });
		mySoundGroups.insert({ SoundTypes::eUI, ui });
		mySoundGroups.insert({ SoundTypes::eAmbiance, ambiance });

		mySoundGroups.at(SoundTypes::eMusic)->setVolume(0.5f);
		mySoundGroups.at(SoundTypes::eSFX)->setVolume(0.5f);
		mySoundGroups.at(SoundTypes::eUI)->setVolume(0.5f);
		mySoundGroups.at(SoundTypes::eAmbiance)->setVolume(0.5f);

		master->addGroup(music);
		master->addGroup(sfx);
		master->addGroup(ui);
		master->addGroup(ambiance);

		mySoundGroups.at(SoundTypes::eMaster)->setVolume(0.5f);

		//
		// 
		//setup events

		myChannels.insert({ EventTypes::eMusic, nullptr });

		//set up banks
		myBanks.insert({ Banks::eMusic, nullptr });

		//load banks
		FMOD::Studio::Bank* musicBank = myBanks.at(Banks::eMusic);
		mySoundContext.system->loadBankFile("../Assets/Audio/Banks/Music.bank", FMOD_STUDIO_LOAD_BANK_NORMAL, &musicBank);


		//

		mySoundContext.system->getBusByID(&FSPRO::Bus::Music, &myChannels.at(EventTypes::eMusic));


		myChannels.at(EventTypes::eMusic)->setVolume(0.5f);

		//


	}
	AudioHandler::~AudioHandler()
	{

		mySoundContext.system->release();
		mySoundContext.coreSystem->release();
	}

	FMOD::Studio::EventInstance* AudioHandler::PlayEvent(const FMOD_GUID anID, const bool /*aPlayOnce*/)
	{
		FMOD::Studio::EventInstance* instance;
		FMOD::Studio::EventDescription* eventDesc;
		mySoundContext.system->getEventByID(&anID, &eventDesc);
		eventDesc->createInstance(&instance);

		instance->start();
		instance->release();
		return instance;
	}

	FMOD::Studio::EventInstance* AudioHandler::PlayEvent(const FMOD_GUID anID, const FMOD_3D_ATTRIBUTES& a3Dattrib, const bool /*aPlayOnce*/)
	{
		FMOD::Studio::EventInstance* instance;
		FMOD::Studio::EventDescription* eventDesc;
		mySoundContext.system->getEventByID(&anID, &eventDesc);
		eventDesc->createInstance(&instance);

		instance->set3DAttributes(&a3Dattrib);

		instance->start();
		instance->release();
		return instance;
	}

	void AudioHandler::ChangeGlobalEventVolume(EventTypes aType, float aVolume)
	{
		auto& bus = myChannels.at(aType);
		bus->setMute(false);

		if (aVolume > 1)
			aVolume = 1;
		else if (aVolume < 0)
			aVolume = 0;

		bus->setVolume(aVolume);
	}

	void AudioHandler::IncrementGlobalEventVolume(EventTypes aType, float aVolume)
	{
		auto& bus = myChannels.at(aType);
		bus->setMute(false);
		float vol = 0;
		bus->getVolume(&vol);

		vol += aVolume;

		if (vol > 1)
			vol = 1;
		else if (vol < 0)
			vol = 0;

		bus->setVolume(vol);
	}

	float AudioHandler::GetGlobalEventVolume(EventTypes aType)
	{
		auto& bus = myChannels.at(aType);
		float vol = 0;;
		bus->getVolume(&vol);

		return vol;
	}

	void AudioHandler::Play(FMOD::Sound*& aSound, SoundTypes aSoundType, FMOD::Channel*& outChannel)
	{
		mySoundContext.coreSystem->playSound(aSound, mySoundGroups[aSoundType], false, &outChannel);
	}

	void AudioHandler::PauseChannel(SoundTypes aSoundType, bool aShouldPause)
	{
		mySoundGroups[aSoundType]->setPaused(aShouldPause);
	}

	void AudioHandler::LoadSound(AudioClip& aClip, bool aShouldLoop, bool aUse3D)
	{
		FMOD_MODE mode = FMOD_DEFAULT;
		mode |= aUse3D ? FMOD_3D : FMOD_2D;
		mode |= aShouldLoop ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;

		FMOD_RESULT result = mySoundContext.coreSystem->createSound(aClip.myPath.c_str(), mode, 0, &aClip.mySound);

		//std::cout << "Load Result: " << result << std::endl;

		if (result != FMOD_RESULT::FMOD_OK)
		{
			std::cout << "FAILDed to load clip " << aClip.myPath << std::endl;
		}
	}

	void AudioHandler::Update()
	{
		if (myListenerTransform)
		{
			SetStaticListener(*myListenerTransform);
		}

		mySoundContext.coreSystem->update();
		mySoundContext.system->update();
	}

	void AudioHandler::ChangeGlobalVolume(SoundTypes aType, float aVolume)
	{
		if (aVolume > 1)
			aVolume = 1;
		else if (aVolume < 0)
			aVolume = 0;

		mySoundGroups[aType]->setVolume(aVolume);
	}

	void AudioHandler::IncrementGlobalVolume(SoundTypes aType, float aVolume)
	{
		float vol = 0;;
		mySoundGroups[aType]->getVolume(&vol);

		vol += aVolume;

		if (vol > 1)
			vol = 1;
		else if (vol < 0)
			vol = 0;

		mySoundGroups[aType]->setVolume(vol);
	}

	float AudioHandler::GetGlobalVolume(SoundTypes aType)
	{
		float vol = 0;;
		mySoundGroups[aType]->getVolume(&vol);

		return vol;
	}

	void AudioHandler::SetStaticListener(const CU::Matrix4x4f& aMatrix)
	{
		CU::Vec3f forward, up;
		forward = aMatrix.GetForward().GetNormalized();
		up = aMatrix.GetUp().GetNormalized();

		myListener.position = { aMatrix.GetTranslationV3().x, aMatrix.GetTranslationV3().y, aMatrix.GetTranslationV3().z };
		myListener.forward = { forward.x, forward.y, forward.z };
		myListener.up = { up.x, up.y, up.z };

		myListener.velocity = { 0,0,0 };

		mySoundContext.coreSystem->set3DListenerAttributes(0, &myListener.position, &myListener.velocity, &myListener.forward, &myListener.up);
		mySoundContext.system->setListenerAttributes(0, &myListener);
	}

	void AudioHandler::HookListener(const CU::Matrix4x4f& aHookMatrix)
	{
		SetStaticListener(aHookMatrix);
		myListenerTransform = &aHookMatrix;
	}

	void AudioHandler::UnHookListener()
	{
		myListenerTransform = nullptr;
	}
	void AudioHandler::PlayPreLoadedSound(PreLoadedSounds aSound)
	{
		SelfAudioClip& clip = myAudioClips.at(aSound);

		//don't know if needed
		if (clip.myClip.myChannel != nullptr)
		{
			
			bool isPlaying;
			clip.myClip.myChannel->isPlaying(&isPlaying);
			if (isPlaying)
				clip.myClip.myChannel->stop();
		}

		mySoundContext.coreSystem->playSound(clip.myClip.mySound, mySoundGroups[clip.myType], false, &clip.myClip.myChannel);
	}
	void AudioHandler::StopPreLoadedSound(PreLoadedSounds aSound)
	{
		SelfAudioClip& clip = myAudioClips.at(aSound);
		if (clip.myClip.myChannel != nullptr)
		{
			clip.myClip.myChannel->stop();
		}
	}
	void AudioHandler::StopAllPreLoadedSounds()
	{
		for (auto& clip : myAudioClips)
		{
			if (clip.second.myClip.myChannel != nullptr)
			{
				clip.second.myClip.myChannel->stop();
			}
		}
	}
}