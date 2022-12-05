#include "AudioManager.h"

namespace ISTE
{
	AudioManager::AudioManager()
	{
		Init();
	}

	AudioManager::~AudioManager()
	{
		myContext.system->release();
	}

	void AudioManager::Init()
	{
		myListenerAttributes = FMOD_3D_ATTRIBUTES();
		myListenerAttributes.up = { 0,1,0 };
		myListenerAttributes.forward = { 0,0,1 };

		time_t now = time(0);
		tm zone;
		localtime_s(&zone, &now);
		if (zone.tm_wday == 5)
		{
			myIsFriday = true;
		}

		myChannels.insert({ Channels::Master, nullptr });
		myChannels.insert({ Channels::Music, nullptr });
		myChannels.insert({ Channels::SFX, nullptr });
		myChannels.insert({ Channels::UI, nullptr });

		FMOD::Studio::System::create(&myContext.system);
		FMOD::System_Create(&myContext.coreSystem);

		myContext.system->getCoreSystem(&myContext.coreSystem);

		myContext.system->initialize(512, FMOD_INIT_NORMAL, 0, nullptr);
		myContext.coreSystem->init(512, FMOD_INIT_NORMAL, 0);

		//myContext.system->loadBankFile("Audio/Master.bank", FMOD_STUDIO_LOAD_BANK_NORMAL, &myMasterBank);
		//myContext.system->loadBankFile("Audio/Music.bank", FMOD_STUDIO_LOAD_BANK_NORMAL, &myMusicBank);
		//myContext.system->loadBankFile("Audio/Player.bank", FMOD_STUDIO_LOAD_BANK_NORMAL, &myPlayerBank);
		//myContext.system->loadBankFile("Audio/UI.bank", FMOD_STUDIO_LOAD_BANK_NORMAL, &myUIBank);
		//myContext.system->loadBankFile("Audio/Enemies.bank", FMOD_STUDIO_LOAD_BANK_NORMAL, &myIntroBank);
		//myContext.system->loadBankFile("Audio/Enviorment.bank", FMOD_STUDIO_LOAD_BANK_NORMAL, &myEnvironmentBank);
		//myContext.system->loadBankFile("Audio/GamePlay.bank", FMOD_STUDIO_LOAD_BANK_NORMAL, &myGamePlayBank);

		

		myContext.system->getBusByID(&FSPRO::Bus::Master_Bus, &myChannels.at(Channels::Master));
		myContext.system->getBusByID(&FSPRO::Bus::Music, &myChannels.at(Channels::Music));
		myContext.system->getBusByID(&FSPRO::Bus::SFX, &myChannels.at(Channels::SFX));
		myContext.system->getBusByID(&FSPRO::Bus::UI, &myChannels.at(Channels::UI));

		

		myChannels.at(Channels::Master)->setVolume(0.5f);
		myChannels.at(Channels::Music)->setVolume(0.5f);
		myChannels.at(Channels::SFX)->setVolume(0.5f);
		myChannels.at(Channels::UI)->setVolume(0.5f);

		myLevelMusic = nullptr;
	}

	void AudioManager::Update()
	{
		if (myListenerTransform)
		{
			CU::Vec3f forward, up;
			forward = myListenerTransform->GetForward().GetNormalized();
			up = myListenerTransform->GetUp().GetNormalized();

			myListenerAttributes.position = { myListenerTransform->GetTranslationV3().x, myListenerTransform->GetTranslationV3().y, myListenerTransform->GetTranslationV3().z };
			myListenerAttributes.forward = { forward.x, forward.y, forward.z };
			myListenerAttributes.up = { up.x, up.y, up.z };

			myContext.system->setListenerAttributes(0, &myListenerAttributes);
		}

		for (auto& c : myAudioClips3D)
			c.Update(0);

		myContext.system->update();
	}

	void AudioManager::SetListenerTransform(const CU::Matrix4x4f& aTransform)
	{
		myListenerTransform = &aTransform;
	}

	void AudioManager::SetListenerTransform(const CU::Matrix4x4f* const aTransform)
	{
		myListenerTransform = aTransform;
	}

	FMOD::Studio::EventInstance* AudioManager::PlayEvent(const FMOD_GUID anID, const bool /*aPlayOnce*/)
	{
		FMOD::Studio::EventInstance* instance;
		FMOD::Studio::EventDescription* eventDesc;
		myContext.system->getEventByID(&anID, &eventDesc);
		eventDesc->createInstance(&instance);

		instance->start();
		instance->release();
		return instance;
	}

	FMOD::Studio::EventInstance* AudioManager::PlayEvent(const FMOD_GUID anID, const FMOD_3D_ATTRIBUTES& a3Dattrib, const bool /*aPlayOnce*/)
	{
		FMOD::Studio::EventInstance* instance;
		FMOD::Studio::EventDescription* eventDesc;
		myContext.system->getEventByID(&anID, &eventDesc);
		eventDesc->createInstance(&instance);

		instance->set3DAttributes(&a3Dattrib);

		instance->start();
		instance->release();
		return instance;
	}


	void AudioManager::StopEvent(const FMOD_GUID anID)
	{
		FMOD::Studio::EventInstance* instance;
		FMOD::Studio::EventDescription* eventDesc;
		myContext.system->getEventByID(&anID, &eventDesc);
		eventDesc->createInstance(&instance);

		instance->stop(FMOD_STUDIO_STOP_ALLOWFADEOUT);
		instance->release();
	}

	void AudioManager::StopAllEvents()
	{
		myChannels.at(Channels::Master)->stopAllEvents(FMOD_STUDIO_STOP_ALLOWFADEOUT);
	}

	void AudioManager::PauseAllEvents()
	{
		myChannels.at(Channels::Master)->setPaused(true);
	}

	void AudioManager::StartAllPausedEvents()
	{
		myChannels.at(Channels::Master)->setPaused(false);
	}



	void AudioManager::SetLevelMusic(int aLevel)
	{
		switch (aLevel)
		{
		case 0:
			myLevelMusic = PlayEvent(FSPRO::Event::MUSIC_Music_Lvl_Intro);
			PlayEvent(FSPRO::Event::INGAME_AMBIENCE_Bird_Ambience);
			break;
		case 1:
		{
			myLevelMusic = PlayEvent(FSPRO::Event::MUSIC_Music_Lvl_1);
			PlayEvent(FSPRO::Event::INGAME_AMBIENCE_Bird_Ambience);
			break;
		}
		case 2:
			myLevelMusic = PlayEvent(FSPRO::Event::MUSIC_Music_Lvl_2);
			PlayEvent(FSPRO::Event::INGAME_AMBIENCE_Bird_Ambience);
			break;
		case 3:
			myLevelMusic = PlayEvent(FSPRO::Event::MUSIC_Music_Lvl_3);
			//PlayEvent(FSPRO::Event::INGAME_AMBIENCE_Cave_Ambience); // replaced with birds because Marco wanted it :)
			PlayEvent(FSPRO::Event::INGAME_AMBIENCE_Bird_Ambience);
			break;
		case 4:
			myLevelMusic = PlayEvent(FSPRO::Event::MUSIC_Music_Hub);
			PlayEvent(FSPRO::Event::INGAME_AMBIENCE_Bird_Ambience);
			break;
		case 6:
			myLevelMusic = PlayEvent(FSPRO::Event::MUSIC_Music_Battle_Ending);
			PlayEvent(FSPRO::Event::INGAME_AMBIENCE_Bird_Ambience);
			break;
		default:
			myLevelMusic->stop(FMOD_STUDIO_STOP_MODE::FMOD_STUDIO_STOP_ALLOWFADEOUT);
			break;
		}
	}

	void AudioManager::SetChannelVolume(const Channels aChannelID, float aVolume)
	{
		auto& bus = myChannels.at(aChannelID);
		bus->setMute(false);
		bus->setVolume(aVolume);
	}

	void AudioManager::SetVolume(float aVolume)
	{
		auto& bus = myChannels.at(Channels::Master);
		bus->setMute(false);
		bus->setVolume(aVolume);
	}

	void AudioManager::MuteAll()
	{
		for (const auto& chan : myChannels)
		{
			chan.second->setMute(true);
		}
	}

	void AudioManager::MuteChannel(const Channels aChannelID, bool aValue)
	{
		myChannels.at(aChannelID)->setMute(aValue);
	}

	void AudioManager::UnmuteChannel(const Channels aChannelID)
	{
		myChannels.at(aChannelID)->setMute(false);
	}

	float AudioManager::GetVolume(const Channels aChannelID) const
	{
		float volume;
		myChannels.at(aChannelID)->getVolume(&volume);
		return volume;
	}

	EventAudioSource* AudioManager::CreateAudioClip()
	{
		auto& clip = myAudioClips3D.emplace_back();
		return &clip;
	}
}