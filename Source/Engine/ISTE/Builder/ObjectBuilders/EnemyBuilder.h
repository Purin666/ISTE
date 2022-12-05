#pragma once

#include "ISTE/Builder/ObjectBuilder.h"

#include "ISTE/Scene/Scene.h"
#include "ISTE/Graphics/ComponentAndSystem/TransformComponent.h"
#include "ISTE/Context.h"
#include "ISTE/Graphics/Resources/ModelManager.h"
#include "ISTE/Physics/ComponentsAndSystems/TriggerComponent.h"

// Enemy behaviours
#include "ISTE/ECSB/IdleEnemyBehaviour.h"
#include "ISTE/ECSB/LurkerEnemyBehaviour.h"
#include "ISTE/ECSB/HunterEnemyBehaviour.h"
#include "ISTE/ECSB/BossBobBehaviour.h"

// Enemy Stats Component
#include "ISTE/ComponentsAndSystems/EnemyStatsComponent.h"

#include "ISTE/Audio/AudioSource.h"

// Json
#include "Json/json.hpp"
#include <fstream>

namespace ISTE
{
	enum class eBehaviours // Must follow enemybehaviours.json's behaviours number system.
	{
		idle,
		lurker,
		hunter,
		bossbob
	};

	class EnemyBuilder : public ObjectBuilder
	{
	public:
		~EnemyBuilder() = default;
		void Create(EntityID aId, GObject& aObject, Scene& aScene) override
		{
			//TransformComponent* t = aScene.AssignComponent<TransformComponent>(aId);
			//t->myPosition = aObject.myTransform.myPosition;
			//t->myQuaternion = aObject.myTransform.myRotation;
			//t->myScale = aObject.myTransform.myScale;
			
			EnemyStatsComponent* stats = aScene.AssignComponent<EnemyStatsComponent>(aId);
			ModelComponent* mc = aScene.GetComponent<ModelComponent>(aId);
			MaterialComponent* matC = aScene.GetComponent<MaterialComponent>(aId);
			mc->myColor *= 2.f;
			matC->myRenderFlags = RenderFlags::Enemies;

			AudioSource* audioSource = aScene.AssignBehaviour<AudioSource>(aId);

			{ // Json Data Import

				{ //---------------Behaviours---------------
					std::ifstream behaviourStream("../Assets/Json/enemybehaviours.json");

					if (!behaviourStream.good())
						return;

					nlohmann::json behaviourReader;
					behaviourStream >> behaviourReader;

					for (nlohmann::json::iterator item = behaviourReader["behaviours"].begin(); item != behaviourReader["behaviours"].end(); item++)
					{
						myStringBehaviours.push_back(item.value());
						myEBehaviours.push_back(eBehaviours(std::stoi(item.key())));
					}

					behaviourReader.clear();
					behaviourStream.close();
				}

				std::ifstream soundStream("../Assets/Json/EnemySounds.json");

				if (!soundStream.good())
				{
					assert("Sound json for enemys could not be found file path = ../Assets/Json/EnemySounds.json");
				}

				nlohmann::json soundJson;
				soundStream >> soundJson;
				soundStream.close();



				{//---------------Enemy Types---------------
					std::ifstream typeStream("../Assets/Json/enemytypes.json");

					if (!typeStream.good())
						return;

					nlohmann::json typeReader;
					typeStream >> typeReader;

					if (typeReader.contains(aObject.myTag))
					{
						for (int i = 0; i < myStringBehaviours.size(); i++)
						{
							if (typeReader[aObject.myTag].contains(myStringBehaviours[i]))
							{
								if (!typeReader[aObject.myTag][myStringBehaviours[i]].get<bool>())
									continue;

								switch (myEBehaviours[i]) // The only hands on stuff.
								{
								case eBehaviours::idle:
								{
									aScene.AssignBehaviour<IdleEnemyBehaviour>(aId);
									break;
								}
								case eBehaviours::lurker:
								{
									LurkerEnemyBehaviour* lEB = aScene.AssignBehaviour<LurkerEnemyBehaviour>(aId);
									TriggerComponent* trigger = aScene.AssignComponent<TriggerComponent>(aId);
									trigger->myOffset = { 0.f, 0.9f, 0.f };
									trigger->mySize = { 0.6f, 1.f, 1.5f };
									AnimatorComponent* animator = aScene.GetComponent<AnimatorComponent>(aId);

									if (animator)
									{
										lEB->myIdleAnim = animator->myAnimations[0];
										lEB->myMovmentAnim = animator->myAnimations[1];
										lEB->myDeadAnim = animator->myAnimations[2];
										lEB->myAttackAnim = animator->myAnimations[3];
									}

									audioSource->LoadAndMapClip((int)LurkerSounds::eAttack,		soundJson["Lurker"]["Attack"]["SoundPath"].get<std::string>(),	   soundJson["Lurker"]["Attack"]["Loop"].get<bool>());
									audioSource->LoadAndMapClip((int)LurkerSounds::eDeath,		soundJson["Lurker"]["Death"]["SoundPath"].get<std::string>(),	   soundJson["Lurker"]["Death"]["Loop"].get<bool>());

									audioSource->LoadAndMapClip((int)SharedSounds::eTookDamage, soundJson["Lurker"]["TookDamage"]["SoundPath"].get<std::string>(), soundJson["Lurker"]["TookDamage"]["Loop"].get<bool>());

									//yucky
									if (aObject.myTag == "EnemyMeleeElite")
									{
										lEB->myIsElite = true;
										stats->myIsElite = true;
									}
									

									//
									break;
								}
								case eBehaviours::hunter:
								{
									HunterEnemyBehaviour* hEB = aScene.AssignBehaviour<HunterEnemyBehaviour>(aId);
									TriggerComponent* trigger = aScene.AssignComponent<TriggerComponent>(aId);
									trigger->myOffset = { 0.f, 0.9f, 0.f };
									trigger->mySize = { 1.2f, 1.f, 0.6f };
									AnimatorComponent* animator = aScene.GetComponent<AnimatorComponent>(aId);

									hEB->myExplodeModel = Context::Get()->myModelManager->LoadModel("../Assets/Animations/CH_NPC_Boris_sack_ANIM.fbx");
									hEB->myExplodeAnim = Context::Get()->myAnimationManager->LoadAnimation(hEB->myExplodeModel, "../Assets/Animations/CH_NPC_Boris_sack_ANIM.fbx");

									if (animator)
									{
										hEB->myIdleAnim = animator->myAnimations[0];
										hEB->myDeadAnim = animator->myAnimations[1];
										hEB->myAttackAnim = animator->myAnimations[2];
										hEB->myMovmentAnim = animator->myAnimations[3];
										hEB->myPreExplodeAnim = animator->myAnimations[4];
										animator->myAnimations[5] = hEB->myExplodeAnim;
									}

									audioSource->LoadAndMapClip((int)HunterSounds::eAttack, soundJson["Hunter"]["Attack"]["SoundPath"].get<std::string>(), soundJson["Hunter"]["Attack"]["Loop"].get<bool>());
									audioSource->LoadAndMapClip((int)HunterSounds::ePreExplosion, soundJson["Hunter"]["PreExplosion"]["SoundPath"].get<std::string>(), soundJson["Hunter"]["PreExplosion"]["Loop"].get<bool>());
									audioSource->LoadAndMapClip((int)HunterSounds::eExplosion, soundJson["Hunter"]["Explosion"]["SoundPath"].get<std::string>(), soundJson["Hunter"]["Explosion"]["Loop"].get<bool>(), true);

									audioSource->LoadAndMapClip((int)SharedSounds::eTookDamage, soundJson["Hunter"]["TookDamage"]["SoundPath"].get<std::string>(), soundJson["Hunter"]["TookDamage"]["Loop"].get<bool>());

									//yucky
									if (aObject.myTag == "EnemyRangedElite")
									{
										hEB->myIsElite = true;
										stats->myIsElite = true;
									}

									//temp
									//
									//
									break;
								}
								case eBehaviours::bossbob:
								{
									aScene.AssignBehaviour<BossBobBehaviour>(aId);
									SphereTriggerComponent* trigger = aScene.AssignComponent<SphereTriggerComponent>(aId);
									trigger->myOffset = { 0.f, 0.9f, 0.f };
									trigger->myRadius = 0.8f;
									aScene.SetBossId(aId);
									break;
								}
								default:
									break;
								}
							}
						}
					}

					typeStream.close();
					typeReader.clear();
				}

				{ //---------------Behaviour Checks---------------
					assert(!((aScene.GetComponent<LurkerEnemyBehaviour>(aId) != nullptr) && (aScene.GetComponent<HunterEnemyBehaviour>(aId) != nullptr)));
				}

			}

			//Context::Get()->myModelManager->LoadModel(aId, aObject.myModelData.myMeshPath);
			//
			//for (int i = 0; i < aObject.myModelData.myAnimations.size(); i++)
			//{
			//	Context::Get()->myModelManager->LoadAnimation(aId, aObject.myModelData.myAnimations[i]);
			//}

			myStringBehaviours.clear();
			myEBehaviours.clear();
		}

	private:

		std::vector<std::string> myStringBehaviours;
		std::vector<eBehaviours> myEBehaviours;

	};
}