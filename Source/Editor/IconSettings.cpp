#include "IconSettings.h"

#include "ISTE/Context.h"
#include "ISTE/ECSB/SystemManager.h"
#include "ISTE/Audio/AudioSourceSystem.h"
#include "ISTE/VFX/SpriteParticles/EmitterSystem.h"
#include "ISTE/Graphics/ComponentAndSystem/LightDrawerSystem.h"

#include "imgui/imgui.h"

namespace ISTE
{
	void IconSettings::StateCheck()
	{

		if (ImGui::MenuItem("DisableAll"))
		{
			myCoverageFlag = 0;
		}

		IconStateFlag fullFlag = (IconStateFlag)FlagValues::eFull;
		if (ImGui::MenuItem("EnableAll"))
		{
			myCoverageFlag = fullFlag;
		}


		bool icon = ((IconStateFlag)FlagValues::eLights & myCoverageFlag);
		ImGui::MenuItem("LightIcons", nullptr, &icon);
		(icon ? myCoverageFlag |= (IconStateFlag)FlagValues::eLights : myCoverageFlag &= (fullFlag ^ (IconStateFlag)FlagValues::eLights));

		icon = ((IconStateFlag)FlagValues::eEmittier & myCoverageFlag);
		ImGui::MenuItem("EmitterIcons", nullptr, &icon);
		(icon ? myCoverageFlag |= (IconStateFlag)FlagValues::eEmittier : myCoverageFlag &= (fullFlag ^ (IconStateFlag)FlagValues::eEmittier));

		icon = ((IconStateFlag)FlagValues::eAudioSource & myCoverageFlag);
		ImGui::MenuItem("AudioIcons", nullptr, &icon);
		(icon ? myCoverageFlag |= (IconStateFlag)FlagValues::eAudioSource : myCoverageFlag &= (fullFlag ^ (IconStateFlag)FlagValues::eAudioSource));

	}

	void IconSettings::DrawIcons()
	{
		SystemManager* sm = Context::Get()->mySystemManager;

		if(myCoverageFlag & (IconStateFlag)FlagValues::eLights)
			sm->GetSystem<LightDrawerSystem>()->PrepareLightIcons();

		if (myCoverageFlag & (IconStateFlag)FlagValues::eEmittier)
			sm->GetSystem<EmitterSystem>()->PrepareEmitterIcons();

		if (myCoverageFlag & (IconStateFlag)FlagValues::eAudioSource)
			sm->GetSystem<AudioSourceSystem>()->PrepareAudioSourceIcons();
	}
}
