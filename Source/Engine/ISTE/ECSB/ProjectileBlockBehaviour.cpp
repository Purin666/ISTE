#include "ProjectileBlockBehaviour.h"

#include "ISTE/Context.h"
#include "ISTE/Scene/Scene.h"
#include "ISTE/Scene/SceneHandler.h"
#include "ISTE/Time/TimeHandler.h"
#include "ISTE/Events/EventHandler.h"

#include "ISTE/ComponentsAndSystems/AttackValueComponent.h"

void ISTE::ProjectileBlockBehaviour::OnTrigger(EntityID aId)
{
	Context* ctx = Context::Get();
	Scene& scene = ctx->mySceneHandler->GetActiveScene();
	AttackValueComponent* attacked = scene.GetComponent<AttackValueComponent>(aId);
	if (attacked == nullptr) // Haha, not attacked this time.
		return;

	if (attacked->myExtraInfo == "Projectile")
	{
		ctx->Get()->myTimeHandler->InvokeTimer("HunterAttack_" + attacked->myIdentifier);
		ctx->Get()->myTimeHandler->RemoveTimer("HunterAttack_" + attacked->myIdentifier);
		ctx->Get()->myTimeHandler->RemoveTimer("HunterProjectileTimer_" + attacked->myIdentifier);
	}

	if (attacked->myTag == "EnemyAttack")
	{
		ctx->myEventHandler->InvokeEvent(EventType::PlayerAbsorbedDamage, (attacked->myAttackValue * 100.f));
		std::cout << "PlayerAbsorbedDamage" << std::endl;
	}




}
