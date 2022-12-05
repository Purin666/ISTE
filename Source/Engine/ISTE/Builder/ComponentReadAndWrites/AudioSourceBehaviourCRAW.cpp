#include "AudioSourceBehaviourCRAW.h"
#include "ISTE/Audio/AudioSource.h"

nlohmann::json ISTE::AudioSourceBehaviourCRAW::Write(void* aComponent, EntityID aEntity)
{
	AudioSource* source = (AudioSource*)aComponent;

	nlohmann::json json;

	nlohmann::json clips;

	int clipCount = 0;
	for (auto& [id, clip] : source->myAudioClips)
	{
		clips[clipCount]["ID"] = id;
		clips[clipCount]["Path"] = clip.myPath;
		clips[clipCount]["Loop"] = clip.myLoop;
		clips[clipCount]["PlayAfterDeath"] = clip.myPlayAfterDeath;
	}

	nlohmann::json audioSource;

	audioSource["Volume"] = source->myVolume;
	audioSource["MinDistance"] = source->myMinDistance;
	audioSource["MaxDistance"] = source->myMaxDistance;

	audioSource["ActiveClip"] = source->myActiveAudioClip;
	audioSource["SoundType"] = (int)source->mySoundType;

	audioSource["PlayOnAwake"] = source->myPlayOnAwake;
	audioSource["Use3DSpace"] = source->myIsUsing3DSpace;


	json["Clips"] = clips;
	json["AudioSource"] = audioSource;

	return json;
}

void ISTE::AudioSourceBehaviourCRAW::Read(void* aComponent, EntityID aEntity, nlohmann::json& aJson)
{
	AudioSource* source = (AudioSource*)aComponent;

	nlohmann::json clips = aJson["Clips"];
	nlohmann::json audioSource = aJson["AudioSource"];

	source->SetSourceVolume(audioSource["Volume"].get<float>());
	source->SetMinDistance(audioSource["MinDistance"].get<float>());
	source->SetMaxDistance(audioSource["MaxDistance"].get<float>());

	source->SetDefaultClip(audioSource["ActiveClip"].get<int>());
	source->SetSoundType((ISTE::SoundTypes)audioSource["SoundType"].get<int>());

	source->SetPlayOnAwake(audioSource["PlayOnAwake"].get<bool>());
	source->Use3DSpace(audioSource["Use3DSpace"].get<bool>());

	size_t clipCount = clips.size();

	for (size_t i = 0; i < clipCount; i++)
	{
		source->LoadAndMapClip
		(
			clips[i]["ID"].get<int>(),
			clips[i]["Path"].get<std::string>(),
			clips[i]["Loop"].get<bool>(),
			clips[i]["PlayAfterDeath"].get<bool>()
		);

	}

}

