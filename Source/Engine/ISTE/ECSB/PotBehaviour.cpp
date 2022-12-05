#include "PotBehaviour.h"

#include "ISTE/Context.h"
#include "ISTE/Scene/Scene.h"
#include "ISTE/Scene/SceneHandler.h"

#include "ISTE/Graphics/ComponentAndSystem/TransformComponent.h"
#include "ISTE/Graphics/ComponentAndSystem/ModelComponent.h"
#include "ISTE/Graphics/ComponentAndSystem/AnimatorComponent.h"
#include "ISTE/ComponentsAndSystems/AttackValueComponent.h"

#include "ISTE/Audio/AudioSource.h"
#include "ISTE/VFX/SpriteParticles/Sprite3DParticleHandler.h"
#include "ISTE/Events/EventHandler.h"

#include "ISTE/CU/MemTrack.hpp"

void ISTE::PotBehaviour::Init()
{
	myActiveFlag = true;
}

void ISTE::PotBehaviour::Update(float aDeltaTime)
{
}

void ISTE::PotBehaviour::OnTrigger(EntityID aId)
{	
	if (!myActiveFlag)
		return;
	
	Scene& scene = Context::Get()->mySceneHandler->GetActiveScene();
	AttackValueComponent* attack = scene.GetComponent<AttackValueComponent>(aId);

	int breakFlag = 0;
	breakFlag += (int)(aId == scene.GetPlayerId());
	if (attack != nullptr)
	{
		breakFlag += (int)(attack->myTag == "PlayerRangedAttack");
		breakFlag += (int)(attack->myTag == "PlayerMeleeAttack");
	}
	if (breakFlag < 1)
		return;

	// break pot
	
	myActiveFlag = false;

	myAnimationHelper.ForcePlay(0);

	//ModelComponent* model = scene.GetComponent<ModelComponent>(myHostId);
	//model->myColor += CU::Vec3f(5.f, 5.f, 5.f);

	Context::Get()->myEventHandler->InvokeEvent(EventType::PotDied, 0);

	TransformComponent* transform = scene.GetComponent<TransformComponent>(myHostId);
	Context::Get()->mySprite3DParticleHandler->SpawnEmitter("Pot_Break_Smoke", transform->myPosition);

	AudioSource* source = Context::Get()->mySceneHandler->GetActiveScene().GetComponent<AudioSource>(myHostId);
	if (source != nullptr)
		source->Play(0);

}