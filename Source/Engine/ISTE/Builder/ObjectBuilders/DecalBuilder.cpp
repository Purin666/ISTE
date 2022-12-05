#include "DecalBuilder.h"
#include "ISTE/Scene/Scene.h"
#include "ISTE/Context.h"
#include "ISTE/Graphics/ComponentAndSystem/DecalComponent.h"
#include "ISTE/Graphics/ComponentAndSystem/ModelComponent.h"
#include "ISTE/Graphics/ComponentAndSystem/TransformComponent.h"
#include "ISTE/Graphics/ComponentAndSystem/CustomShaderComponent.h"
#include "ISTE/Graphics/Resources/ModelManager.h"
#include "ISTE/Math/Quaternion.h"
#include "ISTE/Math/Vec3.h" 
 

ISTE::DecalBuilder::~DecalBuilder()
{
}

void ISTE::DecalBuilder::Create(EntityID aId, GObject& aObject, Scene& aScene)
{
	Context* ctx = Context::Get();
	ModelID mId = ctx->myModelManager->LoadModel(aObject.myModelData.myMeshPath);
	//Model::AABBData aabb = ctx->myModelManager->GetModel(mId)->myAABB;
	//CU::Vec3f size = aabb.myMax - aabb.myMin;
	//
	//TransformComponent* tf = aScene.GetComponent<TransformComponent>(aId); 
	//CU::Vec3f up = tf->myQuaternion.GetUp(); 
	//tf->myQuaternion *= CU::Quaternionf(up, -180.f);
	//tf->myScale.x *= (size.x != 0) ? size.x : 1;
	//tf->myScale.y *= (size.y != 0) ? size.y : 1;
	//tf->myScale.z *= (size.z != 0) ? size.z : 1;
	//
	//aScene.AssignComponent<DecalComponent>(aId);
	auto* csc = aScene.AssignComponent<CustomShaderComponent>(aId);
	csc->myShader = Shaders::eDefaultModelShader;
}
