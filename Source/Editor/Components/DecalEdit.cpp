#pragma once
#include "DecalEdit.h"
#include "imgui/imgui.h"
#include "ISTE/Graphics/ComponentAndSystem/DecalComponent.h"
#include "Commands/CommandFunctions.h"

ISTE::DecalEdit::DecalEdit()
{

}
ISTE::DecalEdit::~DecalEdit()
{

}

void ISTE::DecalEdit::Draw(ISTE::Scene* aScenePointer, Entity anEntity)
{
	ISTE::DecalComponent* decal = aScenePointer->GetComponent<ISTE::DecalComponent>(anEntity.myId);
	if (decal == nullptr)
		return;

	ImGui::Spacing();
	ImGui::InputFloat("Albedo Lerp Value##Decal", &decal->myLerpValues.x);
	ImGui::InputFloat("Normal Lerp Value##Decal", &decal->myLerpValues.y);
	ImGui::InputFloat("Material Lerp Value##Decal", &decal->myLerpValues.z);
	CF::CheckEditing<CU::Vec3f>(myLerpValues, &decal->myLerpValues, okmfse);

	ImGui::Spacing();
	int flags = (int)decal->myCoverageFlag;
	bool drawOnEnv = (flags & (int)RenderFlags::Environment);
	bool drawOnPla = (flags & (int)RenderFlags::Player);
	bool drawOnEne = (flags & (int)RenderFlags::Enemies);
	ImGui::Checkbox("Draw over Environment",	&drawOnEnv);
	ImGui::Checkbox("Draw over Player",			&drawOnPla);
	ImGui::Checkbox("Draw over Enemies",		&drawOnEne); 
	flags = (int)drawOnEnv<<0 | (int)drawOnPla << 1 | (int)drawOnEne << 2; 
	decal->myCoverageFlag = (RenderFlags)flags;

	ImGui::InputFloat("My cuttof threshold##Decal", &decal->myAngleThreshold);
	CF::CheckEditing<float>(myThreshold, &decal->myAngleThreshold, okmfse);
	ImGui::Spacing();
	ImGui::Separator();

}