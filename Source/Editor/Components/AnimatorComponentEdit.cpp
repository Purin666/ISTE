#include "AnimatorComponentEdit.h"
#include "imgui/imgui.h"
#include "ComponentFunctions.h"
#include "ISTE/Graphics/ComponentAndSystem/AnimatorComponent.h"


void ISTE::AnimatorComponentEdit::Draw(ISTE::Scene* aScenePointer, Entity anEntity)
{ 
	ISTE::AnimatorComponent* animatorComponent = aScenePointer->GetComponent<ISTE::AnimatorComponent>(anEntity.myId);
	if (animatorComponent == nullptr)
	{
		return;
	}
	ImGui::Text("Animation Count:");
	ImGui::SameLine();
	ImGui::Text(std::to_string(animatorComponent->myAnimationCount).c_str());
	ImGui::Text("Animation Speed:");
	ImGui::SameLine();
	ImGui::Text(std::to_string(animatorComponent->mySpeedModifier).c_str());
	ImGui::Text("Animation Timer:");
	ImGui::SameLine();
	ImGui::Text(std::to_string(animatorComponent->myTimer).c_str());
	ImGui::Text("Animation Loop:");
	ImGui::SameLine();
	if (ImGui::Button(std::to_string(animatorComponent->myLoopingFlag).c_str()))
	{
		animatorComponent->myLoopingFlag = !animatorComponent->myLoopingFlag;
	}

	{
		std::string correlatedStrings[3] = { "Pause","Play", "End" };
		ISTE::AnimationState animationStates[3] = { AnimationState::ePaused , AnimationState::ePlaying , AnimationState::eEnded };
		EF::SetList<ISTE::AnimationState>((int)animatorComponent->myAnimationState, animatorComponent->myAnimationState, (std::string)"Animation State", correlatedStrings, animationStates);
	}

	ImGui::Separator();
}
