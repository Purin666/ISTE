#pragma once

#include "../ObjectBuilder.h"

#include "../../Scene/Scene.h"
#include "../../Graphics/ComponentAndSystem/TransformComponent.h"
#include "../../Context.h"
#include "../../Graphics/Resources/ModelManager.h"
#include "ISTE/ECSB/PlayerBehaviour.h"
#include "ISTE/Graphics/ComponentAndSystem/AnimatorComponent.h"
#include "ISTE/Graphics/ComponentAndSystem/ModelComponent.h"
#include "ISTE/Audio/AudioSource.h"
#include "ISTE/Audio/AudioHandler.h"

#include "ISTE/Graphics/Resources/TextureManager.h"

//testing
#include "ISTE/Physics/ComponentsAndSystems/TriggerComponent.h"
//

#include "ISTE/CU/ReadWrite.h"

#include <fstream>

namespace ISTE
{
	class PlayerBuilder : public ObjectBuilder
	{
	public:
		void Create(EntityID aId, GObject& aObject, Scene& aScene) override
		{
			//TransformComponent* t = aScene.AssignComponent<TransformComponent>(aId);
			TransformComponent* t = aScene.GetComponent<TransformComponent>(aId);
			PlayerBehaviour* pB = aScene.AssignBehaviour<PlayerBehaviour>(aId);
			AudioSource* source = aScene.AssignBehaviour<AudioSource>(aId);

			//sets the render identifier as player.
			{
				MaterialComponent* mat = aScene.GetComponent<MaterialComponent>(aId);
				mat->myRenderFlags = RenderFlags::Player;
			}

			source->SetSoundType(ISTE::SoundTypes::eSFX);

			{
				nlohmann::json soundJson;
				std::ifstream soundStream("../Assets/Json/PlayerSounds.json");

				struct SoundData
				{
					std::string path;
					bool loop;
					bool activeAfterDeath;
				};

				SoundData data;

				if (soundStream.good())
				{
					soundStream >> soundJson;
					data.path = soundJson["Primary"]["SoundPath"].get<std::string>();
					data.loop = soundJson["Primary"]["Loop"].get<bool>();
					data.activeAfterDeath = soundJson["Primary"]["ActiveAfterDeath"].get<bool>();
					source->LoadAndMapClip((int)PlayerSounds::ePrimaryAttack,data.path, data.loop, data.activeAfterDeath);

					data.path = soundJson["Death"]["SoundPath"].get<std::string>();
					data.loop = soundJson["Death"]["Loop"].get<bool>();
					data.activeAfterDeath = soundJson["Death"]["ActiveAfterDeath"].get<bool>();
					source->LoadAndMapClip((int)PlayerSounds::eDeath,data.path, data.loop, data.activeAfterDeath);

					data.path = soundJson["AOE"]["SoundPath"].get<std::string>();
					data.loop = soundJson["AOE"]["Loop"].get<bool>();
					data.activeAfterDeath = soundJson["AOE"]["ActiveAfterDeath"].get<bool>();
					source->LoadAndMapClip((int)PlayerSounds::eAOE,data.path, data.loop, data.activeAfterDeath);

					data.path = soundJson["Teleport"]["SoundPath"].get<std::string>();
					data.loop = soundJson["Teleport"]["Loop"].get<bool>();
					data.activeAfterDeath = soundJson["Teleport"]["ActiveAfterDeath"].get<bool>();
					source->LoadAndMapClip((int)PlayerSounds::eTeleport,data.path, data.loop, data.activeAfterDeath);

					data.path = soundJson["Secondary"]["SoundPath"].get<std::string>();
					data.loop = soundJson["Secondary"]["Loop"].get<bool>();
					data.activeAfterDeath = soundJson["Secondary"]["ActiveAfterDeath"].get<bool>();
					source->LoadAndMapClip((int)PlayerSounds::eSecondaryAttack, data.path, data.loop, data.activeAfterDeath);

					data.path = soundJson["TookDamage"]["SoundPath"].get<std::string>();
					data.loop = soundJson["TookDamage"]["Loop"].get<bool>();
					data.activeAfterDeath = soundJson["TookDamage"]["ActiveAfterDeath"].get<bool>();
					source->LoadAndMapClip((int)PlayerSounds::eTookDamage, data.path, data.loop, data.activeAfterDeath);

					data.path = soundJson["AOESIMCharge"]["SoundPath"].get<std::string>();
					data.loop = soundJson["AOESIMCharge"]["Loop"].get<bool>();
					data.activeAfterDeath = soundJson["AOESIMCharge"]["ActiveAfterDeath"].get<bool>();
					source->LoadAndMapClip((int)PlayerSounds::eAOESIMCharge, data.path, data.loop, data.activeAfterDeath);

					data.path = soundJson["AOESIMExplode"]["SoundPath"].get<std::string>();
					data.loop = soundJson["AOESIMExplode"]["Loop"].get<bool>();
					data.activeAfterDeath = soundJson["AOESIMExplode"]["ActiveAfterDeath"].get<bool>();
					source->LoadAndMapClip((int)PlayerSounds::eAOESIMExplode, data.path, data.loop, data.activeAfterDeath);

					data.path = soundJson["Walking"]["SoundPath"].get<std::string>();
					data.loop = soundJson["Walking"]["Loop"].get<bool>();
					data.activeAfterDeath = soundJson["Walking"]["ActiveAfterDeath"].get<bool>();
					source->LoadAndMapClip((int)PlayerSounds::eWalking, data.path, data.loop, data.activeAfterDeath);

					data.path = soundJson["MagicArmor"]["SoundPath"].get<std::string>();
					data.loop = soundJson["MagicArmor"]["Loop"].get<bool>();
					data.activeAfterDeath = soundJson["MagicArmor"]["ActiveAfterDeath"].get<bool>();
					source->LoadAndMapClip((int)PlayerSounds::eMagicArmor, data.path, data.loop, data.activeAfterDeath);

					data.path = soundJson["LevelUp"]["SoundPath"].get<std::string>();
					data.loop = soundJson["LevelUp"]["Loop"].get<bool>();
					data.activeAfterDeath = soundJson["LevelUp"]["ActiveAfterDeath"].get<bool>();
					source->LoadAndMapClip((int)PlayerSounds::eLevelUp, data.path, data.loop, data.activeAfterDeath);


				}
				else
				{
					assert("PlayerSounds.json could not be found");
				}

				soundStream.close();
			}

			
			TriggerComponent* trig = aScene.AssignComponent<TriggerComponent>(aId);
			trig->myOffset = { 0, 0.9f, 0 };
			trig->mySize.x *= 0.35f;

			//

			//Setup camera
			const int index = Context::Get()->mySceneHandler->GetActiveSceneIndex();
			const std::string camPath = "../Assets/MiscData/PlayerCamera/PlayerCamera_" + std::to_string(index) + ".cam";
			if (ReadWrite::LoadFile(camPath))
			{
				TransformComponent& camera = pB->GetCameraPositioning();

				CU::Vec3f rot;
				ReadWrite::Read<CU::Vec3f>(rot);
				camera.myEuler.SetRotation(rot);

				CU::Vec3f pos;
				ReadWrite::Read<CU::Vec3f>(pos);
				camera.myPosition = pos;

				float fov = 0;
				ReadWrite::Read<float>(fov);

				pB->GetCameraPositioning().myScale.x = fov;
				pB->GetCamera().SetPerspectiveProjection(fov, Context::Get()->myWindow->GetResolution(), 0.5f, 5000);

				ReadWrite::Flush();
			}

			//

			aScene.SetPlayerId(aId);
			t->myPosition = aObject.myTransform.myPosition;
			t->myQuaternion = aObject.myTransform.myRotation;
			t->myScale = aObject.myTransform.myScale;

			ModelID mId = aScene.GetComponent<ModelComponent>(aId)->myModelId;

			
			AnimatorComponent* animator = aScene.GetComponent<AnimatorComponent>(aId);

			if (animator)
			{
				pB->myIdleId = animator->myAnimations[0];
				pB->myWalkId = animator->myAnimations[1];
				pB->myDeadId = animator->myAnimations[2];
				pB->myThrowAbilityId = animator->myAnimations[3];
				pB->myMeleeAttackId = animator->myAnimations[4];
				pB->mySpellId = animator->myAnimations[5];
			}

			//pB->myIdleId = Context::Get()->myModelManager->LoadAnimation(aId, "../Assets/Animations/CH_Player_idle_ANIM.fbx").myValue;
			//pB->myWalkId = Context::Get()->myModelManager->LoadAnimation(aId, "../Assets/Animations/CH_Player_walk_ANIM.fbx").myValue;
			//pB->myDeadId = Context::Get()->myModelManager->LoadAnimation(aId, "../Assets/Animations/CH_Player_Dead_ANIM.fbx");
			//pB->myThrowAbilityId = Context::Get()->myModelManager->LoadAnimation(aId, "../Assets/Animations/CH_Player_ThrowAbility_ANIM.fbx");

			//ModelID mId = Context::Get()->myModelManager->LoadModel(aId, aObject.myModelData.myMeshPath).myValue;
			//
			//for (int i = 0; i < aObject.myModelData.myAnimations.size(); i++)
			//{
			//	Context::Get()->myModelManager->LoadAnimation(aId, aObject.myModelData.myAnimations[i]);
			//}

			pB->myRightHandBoneIndex = Context::Get()->myModelManager->GetBoneNameToIdMap(mId)["RightHandWeapon_SK"];
			pB->myLeftHandNoneIndex = Context::Get()->myModelManager->GetBoneNameToIdMap(mId)["LeftHandWeapon_SK"];;

			pB->myWeaponId = aScene.NewEntity();
			TransformComponent* mT = aScene.AssignComponent<TransformComponent>(pB->myWeaponId);
			ModelComponent* mM = aScene.AssignComponent<ModelComponent>(pB->myWeaponId);
			MaterialComponent* mMat = aScene.AssignComponent<MaterialComponent>(pB->myWeaponId);
			mM->myModelId = Context::Get()->myModelManager->LoadModel("../Assets/Models/Characters/CH_WP_Axe.fbx");

			pB->myWeaponHoldBone = Context::Get()->myModelManager->GetBoneNameToIdMap(mM->myModelId)["HoldJoint_Axe_SK"];;

			Model* m = Context::Get()->myModelManager->GetModel(mM->myModelId);
			memcpy(mMat->myTextures, m->myTextures, sizeof(TextureID) * MAX_MESH_COUNT * MAX_MATERIAL_COUNT);
			mMat->myRenderFlags = RenderFlags::Player;
			mMat->myTextures[0][ALBEDO_MAP] = Context::Get()->myTextureManager->LoadTexture(L"../Assets/Models/Characters/CH_WP_Axe_c.dds", true);
			mMat->myTextures[0][MATERIAL_MAP] = Context::Get()->myTextureManager->LoadTexture(L"../Assets/Models/Characters/CH_WP_Axe_m.dds");
			mMat->myTextures[0][NORMAL_MAP] = Context::Get()->myTextureManager->LoadTexture(L"../Assets/Models/Characters/CH_WP_Axe_n.dds");


			mT->myScale = { 1,1,1 };
			aScene.SetEntityName(pB->myWeaponId, "PlayerWeapon");

			//// Sphere for vfx
			//{
			//	EntityID sphereID = aScene.NewEntity();

			//	pB->SetSphereID(sphereID);
			//
			//	Context::Get()->myModelManager->LoadModel(sphereID, "../Assets/Models/HiPoly_Ico_Sphere.fbx");
			//}
		}

	private:
		
	};
}