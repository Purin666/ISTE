#include "EmitterComponentEdit.h"

#include "imgui/imgui.h"
#include "imgui/misc/cpp/imgui_stdlib.h"

#include "ISTE/Context.h"

#include "ISTE/Scene/SceneHandler.h"
#include "ISTE/Scene/Scene.h"

#include "ISTE/VFX/SpriteParticles/EmitterComponent.h"
#include "ISTE/Graphics/ComponentAndSystem/TransformComponent.h"

#include "ISTE/VFX/SpriteParticles/Sprite3DParticleHandler.h"

void ISTE::EmitterComponentEdit::Draw(ISTE::Scene* aScenePointer, Entity anEntity)
{
	if (!aScenePointer->IsEntityIDValid(anEntity.myId))
		return;
	
	EmitterComponent*	e = aScenePointer->GetComponent<EmitterComponent>(anEntity.myId);
	TransformComponent* t = aScenePointer->GetComponent<TransformComponent>(anEntity.myId);

	if (e == nullptr || t == nullptr)
		return;

	Context* ctx = Context::Get();
	Sprite3DParticleHandler* sh = ctx->mySprite3DParticleHandler;

	ImGui::InputText("Emitter Type Name", &e->myEmitterType);

	if (ImGui::Button("Spawn"))
	{
		e->myEmitterId = sh->SpawnEmitter(e->myEmitterType, t->myPosition);
	}
	if (e->myEmitterId > -1)
	{
		if (ImGui::Button("Deactivate"))
		{
			sh->DeactivateEmitter(e->myEmitterId);
			e->myEmitterId = -1;
		}
		if (ImGui::Button("Kill"))
		{
			sh->KillEmitter(e->myEmitterId);
			e->myEmitterId = -1;
		}
	}
	ImGui::Separator();
}
