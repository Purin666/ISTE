#include "BehaviourEdit.h"
#include "ISTE/ECSB/Behaviour.h"
#include "imgui/imgui.h"

void ISTE::BehaviourEdit::Draw(ISTE::Scene* aScenePointer, Entity anEntity)
{
	ISTE::BehaviourHandle behaviour = aScenePointer->GetBehaviourHandle(anEntity.myId);
	if (behaviour.GetEverything().size() > 0)
	{
		ImGui::Text("Behaviour:");
	}

	for (ISTE::Behaviour* i : behaviour.GetEverything())
	{
		ImGui::Text(typeid(*i).name());
	}
	ImGui::Separator();
}
