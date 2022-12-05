#include "AudioSourceBehaviourEdit.h"

#include "imgui/imgui.h"
#include "imgui/misc/cpp/imgui_stdlib.h"

#include "ISTE/Audio/AudioSource.h"

#include "ISTE/Context.h"
#include "ISTE/Time/TimeHandler.h"

void ISTE::AudioSourceBehaviourEdit::Draw(ISTE::Scene* aScenePointer, Entity anEntity)
{ 
	if (!aScenePointer->IsEntityIDValid(anEntity.myId))
		return;

	AudioSource* audioSource = aScenePointer->GetComponent<AudioSource>(anEntity.myId);

	if (audioSource == nullptr)
		return;

	if (myErrorText != "")
	{
		myTimer += Context::Get()->myTimeHandler->GetDeltaTime();

		if (myTimer >= myErrorTime)
		{
			myTimer = 0;
			myErrorText = "";
		}
	}

	bool hasAudio = audioSource->myAudioClips.size();

	if (hasAudio && myClipEditPath == "")
	{
		myClipEditPath = audioSource->myAudioClips.at(0).myPath;
		myClipPath = myClipEditPath;
	}

	ImGui::Separator();
	ImGui::Text("AudioSource");

	ImGui::Text("ClipEdit");

	ImGui::InputText("Clip", &myClipEditPath);

	if (ImGui::IsItemDeactivatedAfterEdit())
	{
		if (myClipPath != myClipEditPath)
		{
			myClipPath = myClipEditPath;
			audioSource->UnLoadClip(0);
			if (!audioSource->LoadAndMapClip(0, myClipPath, false))
				myErrorText = "Could not load clip";
		}

	}

	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(".wav", 0))
		{
			myClipPath = *(std::string*)payload->Data;

			if (myClipPath != myClipEditPath)
			{
				myClipEditPath = myClipPath;
				audioSource->UnLoadClip(0);
				if (!audioSource->LoadAndMapClip(0, myClipPath, false))
					myErrorText = "Could not load clip";
			}
		}
		else if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(".mp3", 0))
		{
			myClipPath = *(std::string*)payload->Data;

			if (myClipPath != myClipEditPath)
			{
				myClipEditPath = myClipPath;
				audioSource->UnLoadClip(0);
				if (!audioSource->LoadAndMapClip(0, myClipPath))
					myErrorText = "Could not load clip";
			}
		}
	}

	hasAudio = audioSource->myAudioClips.size();

	if (hasAudio)
	{
		bool loop = audioSource->myAudioClips.at(0).myLoop;

		ImGui::Checkbox("Loop Audio", &loop);
		
		if (ImGui::IsItemEdited())
		{
			audioSource->UnLoadClip(0);
			audioSource->LoadAndMapClip(0, myClipPath, loop);
		}

		audioSource->Update(0);
	}
	ImGui::Text("SourceEdit");
	float volume = audioSource->myVolume;
	ImGui::SliderFloat("Volume", &volume, 0, 1);
	audioSource->SetSourceVolume(volume);

	float minMax[2] = { audioSource->myMinDistance, audioSource->myMaxDistance };
	ImGui::InputFloat2("MinMax", minMax); //spoopy

	audioSource->SetMinDistance(minMax[0]);
	audioSource->SetMaxDistance(minMax[1]);

	SoundTypes soundTypes[(int)SoundTypes::eCount];

	soundTypes[0] = SoundTypes::eMaster;
	soundTypes[1] = SoundTypes::eAmbiance;
	soundTypes[2] = SoundTypes::eMusic;
	soundTypes[3] = SoundTypes::eSFX;
	soundTypes[4] = SoundTypes::eUI;

	std::string soundTypesName[(int)SoundTypes::eCount];

	soundTypesName[0] = "Master";
	soundTypesName[1] = "Ambience";
	soundTypesName[2] = "Music";
	soundTypesName[3] = "SFX";
	soundTypesName[4] = "UI";

	
	int mapped = 0;

	for (int i = 0; i < (int)SoundTypes::eCount; i++)
	{
		if (soundTypes[i] == audioSource->mySoundType)
		{
			mapped = i;
			break;
		}
	}


	std::string selectedName = soundTypesName[mapped];

	if (ImGui::BeginCombo("Type", selectedName.c_str()))
	{
		for (int i = 0; i < (int)SoundTypes::eCount; i++)
		{
			const bool selected = (selectedName == soundTypesName[i]);

			if (ImGui::Selectable(soundTypesName[i].c_str(), selected))
			{
				audioSource->SetSoundType(soundTypes[i]);
			}

					if (selected)
						ImGui::SetItemDefaultFocus();

		}

		ImGui::EndCombo();
	}

	ImGui::Checkbox("PlayOnAwake", &audioSource->myPlayOnAwake);

	bool use3DSpace = audioSource->myIsUsing3DSpace;
	ImGui::Checkbox("Use3DSpace", &use3DSpace);
	
	if (use3DSpace != audioSource->myIsUsing3DSpace)
	{
		audioSource->Use3DSpace(use3DSpace);
	}

	ImGui::Text("Actions");

	if (ImGui::Button("Play Sound"))
	{
		hasAudio = audioSource->myAudioClips.size();
		if (hasAudio)
		{
			audioSource->Play(0);
		}
		else
		{
			myErrorText = "No Audio was loaded";
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("Stop Sound"))
	{
		audioSource->Stop(0);
	}
	ImGui::SameLine();
	if (ImGui::Button("Pause Sound"))
	{
		audioSource->Pause(0, !audioSource->IsPlaying(0));
	}

	if (myErrorText != "")
	{

		float color = 255;

		if (myTimer >= myStartFadeTime)
		{
			color = 255 - (255 * (myTimer / myErrorTime));
		}

		ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, color));
		ImGui::Text(myErrorText.c_str());
		ImGui::PopStyleColor();
	}

	ImGui::Separator(); 
}
