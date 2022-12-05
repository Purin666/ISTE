#include "Scene.h"

#include <iostream>
#include <Windows.h>
#include "../Engine/ISTE/WindowsWindow.h"

#include "ISTE/CU/MemTrack.hpp"

#include "ISTE/Events/EventHandler.h"
//If this should not be here feel free to tell me. -Loke
#include "Json/json.hpp"
#include <fstream>
//temp
#include "ISTE/Graphics/ComponentAndSystem/TransformComponent.h"
#include "ISTE/Graphics/ComponentAndSystem/TransformSystem.h"
#include "ISTE/ECSB/BobStructs.h"

#include "ISTE/CU/Helpers.h"

namespace ISTE
{
	Scene::~Scene()
	{
		for (std::vector<int>* i : myTemporaryStorage)
		{
			delete i;
		}
		for (auto& entity : myEntities)
		{
			for (auto* behaviour : entity.myBehaviourHandle.GetEverything())
			{
				behaviour->~Behaviour();
			}

			entity.myBehaviourHandle.GetEverything().clear();

			//this should probably be handled in a better way
			for (ComponentID cID = 0; cID < MAX_COMPONENTS; cID++)
			{
				if (mySceneHandler->myFCM.test(cID) && entity.myMask.test(cID))
				{
					void* cmp = myComponentPools[cID]->Get(GetEntityIndex(entity.myId));
					mySceneHandler->mySimpleCalls.at(cID).myDesCall(cmp);
				}
			}
		}

		for (auto& pair : myComponentPools)
		{
			delete pair.second;
		}

		myComponentPools.clear();
	}
	EntityID Scene::NewEntity()
	{
		if (myFreeIndexes.size() != 0)
		{
			EntityID id = CreateEntityId(myFreeIndexes[0], GetEntityVersion(myEntities[myFreeIndexes[0]].myId));
			myFreeIndexes.erase(myFreeIndexes.begin());

			myEntities[GetEntityIndex(id)].myId = id;

			Context::Get()->mySystemManager->EntitySignatureChanged(id, myEntities[GetEntityIndex(id)].myMask, true);

			return id;
		}


		//if (myEntities.size() == MAX_ENTITIES)
		//{
		//	return INVALID_ENTITY;
		//}

		ASSERT_WITH_MSG(myEntities.size() != MAX_ENTITIES, ((L"Entity limit reached (" + std::to_wstring(MAX_ENTITIES)) + L")").c_str());

		Entity ent;
		ent.myId = CreateEntityId((EntityIndex)myEntities.size(), 0);
		myEntities.push_back(ent);

		Context::Get()->mySystemManager->EntitySignatureChanged(myEntities.back().myId, myEntities[GetEntityIndex(myEntities.back().myId)].myMask, true);

		return myEntities.back().myId;
	}

	void Scene::DestroyEntity(EntityID aId)
	{
		if (!IsEntityIDValid(aId))
			return;

		myEntitiesToDestroy.push_back(aId);
		/*EntityIndex index = GetEntityIndex(aId);

		if (index == (EntityIndex)-1)
			return;

		EntityID newID = CreateEntityId(EntityIndex(-1), GetEntityVersion(aId) + 1);

		for (size_t i = 0; i < myEntities[index].myMask.size(); i++)
		{
			if (myEntities[index].myMask.test(i))
			{
				myComponentPools[i]->RemoveMapping(index);
			}
		}

		myEntities[index].myId = newID;
		myEntities[index].myMask.reset();

		if (myEntities[index].myParent != INVALID_ENTITY)
		{
			UnParent(aId, myEntities[index].myParent);
		}

		for (auto& child : myEntities[index].myChildren)
		{
			DestroyEntity(child);
		}

		for (auto* behaviour : myEntities[index].myBehaviourHandle.GetEverything())
		{
			behaviour->~Behaviour();
		}

		myEntities[index].myBehaviourHandle.GetEverything().clear();

		myFreeIndexes.push_back(index);

		Context::Get()->mySystemManager->EntityDestroyed(aId);*/
	}
	void Scene::DeactivateEntity(EntityID aId, bool aCullArg)
	{
		if (!IsEntityIDValid(aId))
			return;

		DeactivationArg arg;
		arg.myCullArg = aCullArg;
		arg.myId = aId;

		myEntitiesDeactivated.push_back(arg);

	}
	void Scene::ActivateEntity(EntityID aId, bool aCullArg)
	{
		if (!IsEntityIDValid(aId))
			return;

		EntityIndex index = GetEntityIndex(aId);

		if (myEntities[index].myIsActive == true)
			return;

		if (!myEntities[index].myWasCulled && !aCullArg)
		{
			myEntities[index].myIsActive = true;
		}
		else if (myEntities[index].myWasCulled && aCullArg)
		{
			myEntities[index].myIsActive = true;
			myEntities[index].myWasCulled = false;
		}

		Context::Get()->mySystemManager->EntityActivated(aId, myEntities[index].myMask, (myEntities[index].myParent == INVALID_ENTITY));
		Context::Get()->myEventHandler->InvokeEvent(EventType::EntityActivated, aId);
	}
	void Scene::Update(float aTimeDelta)
	{

	}
	void Scene::SetParent(EntityID aChildID, EntityID aParentID, bool aTakeToParentSpace)
	{
		Entity& parent = myEntities[GetEntityIndex(aParentID)];
		Entity& child = myEntities[GetEntityIndex(aChildID)];

		//for now the childing stuff will be pretty ugly
		if (child.myParent != INVALID_ENTITY)
		{
			Entity& cParent = myEntities[GetEntityIndex(child.myParent)];
			UnParent(child.myId, cParent.myId);
		}

		child.myParent = parent.myId;
		parent.myChildren.push_back(child.myId);

		if (aTakeToParentSpace)
		{
			Context::Get()->myEventHandler->InvokeEvent(ISTE::EventType::EntityParentedToSpace, aChildID);
		}

		Context::Get()->myEventHandler->InvokeEvent(ISTE::EventType::EntityParented, aChildID);

		////temp
		//if (aTakeToParentSpace)
		//{
		//	TransformComponent* cCMP = GetComponent<TransformComponent>(child.myId);
		//	TransformComponent* pCMP = GetComponent<TransformComponent>(parent.myId);
		//	if (cCMP != nullptr && pCMP != nullptr)
		//	{
		//		Context::Get()->mySystemManager->GetSystem<TransformSystem>()->UpdateToSpace(pCMP, cCMP, child.myId);
		//	}
		//}
		////

	}
	void Scene::UnParent(EntityID aChildID, EntityID aParentID)
	{

		Entity& parent = myEntities[GetEntityIndex(aParentID)];
		Entity& child = myEntities[GetEntityIndex(aChildID)];

		for (int i = 0; i < parent.myChildren.size(); i++)
		{
			if (parent.myChildren[i] == child.myId)
			{
				parent.myChildren.erase(parent.myChildren.begin() + i);
				break;
			}
		}

		Context::Get()->myEventHandler->InvokeEvent(ISTE::EventType::EntityUnParented, aChildID);

		child.myParent = INVALID_ENTITY;

		////temp
		//TransformComponent* cCMP = GetComponent<TransformComponent>(child.myId);
		//TransformComponent* pCMP = GetComponent<TransformComponent>(parent.myId);
		//if (cCMP != nullptr && pCMP != nullptr)
		//{
		//	Context::Get()->mySystemManager->GetSystem<TransformSystem>()->UpdateFromSpace(pCMP, cCMP, child.myId);
		//}
		////

	}
	void Scene::CreateComponentPool(ComponentID aId, size_t aSize)
	{
		size_t cSize = mySceneHandler->myRegisteredComponents[aId];

		myComponentPools.insert({ aId, new ComponentPool(cSize, aSize, MAX_ENTITIES) });
	}

	//All removes goes through here
	void Scene::PostUpdateCalls()
	{
		//destroy
		for (auto& entId : myEntitiesToDestroy)
		{
			PostDestroy(entId);
		}

		myEntitiesToDestroy.clear();

		//deactivate
		for (auto& arg : myEntitiesDeactivated)
		{
			PostDeactivated(arg);
		}

		myEntitiesDeactivated.clear();

		//Component removed
		for (auto& arg : myComponentsRemoved)
		{
			PostComponentRemove(arg);
		}

		myComponentsRemoved.clear();

		//behaviour removed
		for (auto& arg : myBehavioursRemoved)
		{
			PostBehaviourRemove(arg);
		}

		myBehavioursRemoved.clear();


	}

	void Scene::PostDestroy(EntityID aID)
	{
		if (!IsEntityIDValid(aID))
			return;

		EntityIndex index = GetEntityIndex(aID);
		EntityID newID = CreateEntityId(EntityIndex(-1), GetEntityVersion(aID) + 1);

		if (myEntities[index].myParent != INVALID_ENTITY)
		{
			UnParent(aID, myEntities[index].myParent);
		}

		for(int i = myEntities[index].myChildren.size() - 1; i >= 0; i--)
		{
			PostDestroy(myEntities[index].myChildren[i]);
		}

		for (size_t i = 0; i < myEntities[index].myMask.size(); i++)
		{
			if (myEntities[index].myMask.test(i))
			{
				//right now only components have simple calls destructor
				if (mySceneHandler->mySimpleCalls[i].myDesCall != nullptr)
				{
					mySceneHandler->mySimpleCalls[i].myDesCall(myComponentPools[i]->Get(index));
				}

				myComponentPools[i]->RemoveMapping(index);
			}
		}

		for (auto* behaviour : myEntities[index].myBehaviourHandle.GetEverything())
		{
			behaviour->~Behaviour();
		}

		myEntities[index].myBehaviourHandle.GetEverything().clear();


		myEntities[index] = Entity();
		myEntities[index].myId = newID;

		myFreeIndexes.push_back(index);

		Context::Get()->mySystemManager->EntityDestroyed(aID);
		Context::Get()->myEventHandler->InvokeEvent(EventType::EntityDestroyed, aID);
	}

	void Scene::PostDeactivated(DeactivationArg aArg)
	{
		if (!IsEntityIDValid(aArg.myId))
			return;

		EntityIndex index = GetEntityIndex(aArg.myId);

		if (myEntities[index].myIsActive == false)
			return;

		myEntities[index].myIsActive = false;
		myEntities[index].myWasCulled = aArg.myCullArg;

		Context::Get()->mySystemManager->EntityDeactivated(aArg.myId);
		Context::Get()->myEventHandler->InvokeEvent(EventType::EntityDeactivated, aArg.myId);
	}

	void Scene::PostComponentRemove(RemoveComponentArg aArg)
	{
		if (!IsEntityIDValid(aArg.myId))
			return;

		EntityIndex index = GetEntityIndex(aArg.myId);

		void* cmp = myComponentPools[aArg.myComponentId]->RemoveMapping(index);

		if (mySceneHandler->myFCM.test(aArg.myComponentId) && cmp != nullptr)
			mySceneHandler->mySimpleCalls.at(aArg.myComponentId).myDesCall(cmp);

		myEntities[index].myMask.set(aArg.myComponentId, false);

		if (myEntities[index].myIsActive)
			Context::Get()->mySystemManager->EntitySignatureChanged(aArg.myId, myEntities[index].myMask, (myEntities[index].myParent == INVALID_ENTITY));

	}

	void Scene::PostBehaviourRemove(RemoveComponentArg aArg)
	{
		if (!IsEntityIDValid(aArg.myId))
			return;

		EntityIndex index = GetEntityIndex(aArg.myId);

		void* cmp = myComponentPools[aArg.myComponentId]->RemoveMapping(index);

		if (mySceneHandler->myFCM.test(aArg.myComponentId) && cmp != nullptr)
			mySceneHandler->mySimpleCalls.at(aArg.myComponentId).myDesCall(cmp);

		myEntities[index].myMask.set(aArg.myComponentId, false);

		if (myEntities[index].myIsActive)
			Context::Get()->mySystemManager->EntitySignatureChanged(aArg.myId, myEntities[index].myMask, (myEntities[index].myParent == INVALID_ENTITY));

		std::vector<Behaviour*>& b = myEntities[index].myBehaviourHandle.GetEverything();

		int count = 0;
		for (auto& behaviour : b)
		{
			if (behaviour->myComponentId == aArg.myComponentId)
			{
				behaviour->~Behaviour();
				b.erase(b.begin() + count);
				break;
			}
			count++;
		}
	}

	void Scene::LoadBehaviours()
	{
		while (myTemporaryStorage.size() > 0)
		{
			delete myTemporaryStorage.front();
			myTemporaryStorage.pop_front();
		}
		{ // Json Data Import

			std::ifstream stream("../Assets/Json/enemybehaviours.json");

			if (stream)
			{
				nlohmann::json reader;
				stream >> reader;
				//Idle
				myBehaviours.SetValue("IdleRadius", reader["idle"]["radius"].get<float>());
				myBehaviours.SetValue("IdleSpeed", reader["idle"]["speed"].get<float>());
				myBehaviours.SetValue("IdleRest", reader["idle"]["resttime"].get<float>());
				myBehaviours.SetValue("IdleDamageDuration", reader["idle"]["damageshaderduration"].get<float>());
				myBehaviours.SetValue("IdleHealth", reader["idle"]["health"].get<float>());
				// Hunter
				myBehaviours.SetValue("HunterSpeed", reader["hunter"]["speed"].get<float>());
				myBehaviours.SetValue("HunterFindRadius", reader["hunter"]["findradius"].get<float>());
				myBehaviours.SetValue("HunterWatchTime", reader["hunter"]["watchtime"].get<float>());
				myBehaviours.SetValue("HunterKeepDistance", reader["hunter"]["keepdistance"].get<float>());
				myBehaviours.SetValue("HunterHealth", reader["hunter"]["health"].get<float>());

				myBehaviours.SetValue("HunterExplosionDistance", reader["hunter"]["explosionstats"]["explosiondistance"].get<float>());
				myBehaviours.SetValue("HunterWarningDelay", reader["hunter"]["explosionstats"]["warningdelay"].get<float>());
				myBehaviours.SetValue("HunterExplosionRange", reader["hunter"]["explosionstats"]["explosionrange"].get<float>());
				myBehaviours.SetValue("HunterExplosionDamage", reader["hunter"]["explosionstats"]["explosiondamage"].get<float>());

				myBehaviours.SetValue("HunterWeaponDamage", reader["hunter"]["weapon"]["damage"].get<float>());
				myBehaviours.SetValue("HunterWeaponSize", reader["hunter"]["weapon"]["size"].get<float>());
				myBehaviours.SetValue("HunterWeaponAttackSpeed", reader["hunter"]["weapon"]["attackspeed"].get<float>());
				myBehaviours.SetValue("HunterWeaponStay", reader["hunter"]["weapon"]["stayrate"].get<float>());
				myBehaviours.SetValue("HunterWeaponSpeed", reader["hunter"]["weapon"]["speed"].get<float>());
				myBehaviours.SetValue("HunterWeaponMinDistance", reader["hunter"]["weapon"]["minimumdistance"].get<float>());

				myBehaviours.SetValue("HunterName", reader["hunter"]["name"].get<std::string>());

				myBehaviours.SetValue("HunterExperience", reader["hunter"]["experience"].get<int>());

				// Lurker
				myBehaviours.SetValue("LurkerSpeed", reader["lurker"]["speed"].get<float>());
				myBehaviours.SetValue("LurkerFindRadius", reader["lurker"]["findradius"].get<float>());
				myBehaviours.SetValue("LurkerWatchTime", reader["lurker"]["watchtime"].get<float>());
				myBehaviours.SetValue("LurkerKeepDistance", reader["lurker"]["keepdistance"].get<float>());
				myBehaviours.SetValue("LurkerHealth", reader["lurker"]["health"].get<float>());

				myBehaviours.SetValue("LurkerWeaponDamage", reader["lurker"]["weapon"]["damage"].get<float>());
				myBehaviours.SetValue("LurkerWeaponSize", reader["lurker"]["weapon"]["size"].get<float>());
				myBehaviours.SetValue("LurkerWeaponAttackSpeed", reader["lurker"]["weapon"]["attackspeed"].get<float>());
				myBehaviours.SetValue("LurkerWeaponStay", reader["lurker"]["weapon"]["stayrate"].get<float>());

				myBehaviours.SetValue("LurkerName", reader["lurker"]["name"].get<std::string>());

				myBehaviours.SetValue("LurkerExperience", reader["lurker"]["experience"].get<int>());
				//Boss bob
				AcidSpit acidSpit;
				StabSweep stabSweep;
				StabAndStuck stabAndStuck;
				RoarHelp roarHelp;
				myBehaviours.SetValue("BossHealth", reader["bossbob"]["health"].get<float>());
				myBehaviours.SetValue("BossDamageDuration", reader["bossbob"]["damageshaderduration"].get<float>());
				myBehaviours.SetValue("BossName", reader["bossbob"]["name"].get<std::string>());
				myBehaviours.SetValue("BossRotationSpeed", reader["bossbob"]["rotationspeed"].get<float>());
				int number = 0;
				for (nlohmann::json i : reader["bossbob"]["stages"])
				{
					myTemporaryStorage.push_back(new std::vector<int>);
					for (int j : i["attackoptions"])
					{
						myTemporaryStorage.back()->push_back(j);
					}

					myBehaviours.SetValue("AttackOptions" + std::to_string(number), myTemporaryStorage.back());
					myBehaviours.SetValue("orderdelay" + std::to_string(number), i["orderdelay"].get<float>());

					//AcidSpit
					acidSpit.Damage = i["acidspit"]["damage"].get<float>();
					acidSpit.StayTime = i["acidspit"]["staytime"].get<float>();
					acidSpit.PrewarningTime = i["acidspit"]["prewarningtime"].get<float>();
					acidSpit.SizePerPuddle = i["acidspit"]["sizeperpuddle"].get<float>();
					acidSpit.SizePerSpit = i["acidspit"]["sizeperspit"].get<float>();
					acidSpit.RandomAcidSpitPlacementRange = i["acidspit"]["randomacidspitplacementrange"].get<float>();
					acidSpit.DelayBetweenSpits = i["acidspit"]["delaybetweenspits"].get<float>();
					acidSpit.DelayAfterAllShots = i["acidspit"]["delayafterallshots"].get<float>();
					acidSpit.ExtraHeight = i["acidspit"]["extraheight"].get<float>();
					acidSpit.SpitCount = i["acidspit"]["spitcount"].get<int>();
					acidSpit.SpitsLeft = acidSpit.SpitCount;
					acidSpit.AttackSource = {
						i["acidspit"]["attacksource"][0].get<float>(),
						i["acidspit"]["attacksource"][1].get<float>(),
						i["acidspit"]["attacksource"][2].get<float>() };
					myBehaviours.SetValue("AcidSpit" + std::to_string(number), acidSpit);
					//StabSweep
					stabSweep.Damage = i["stabsweep"]["damage"].get<float>();
					stabSweep.Speed = i["stabsweep"]["speed"].get<float>();
					stabSweep.SizeOnSweep = i["stabsweep"]["sizeonsweep"].get<float>();
					stabSweep.ForwarningTime = i["stabsweep"]["forwarningtime"].get<float>();
					stabSweep.LeftPoint = {
						i["stabsweep"]["leftpoint"][0].get<float>(),
						i["stabsweep"]["leftpoint"][1].get<float>(),
						i["stabsweep"]["leftpoint"][2].get<float>()
					};
					stabSweep.RightPoint = {
						i["stabsweep"]["rightpoint"][0].get<float>(),
						i["stabsweep"]["rightpoint"][1].get<float>(),
						i["stabsweep"]["rightpoint"][2].get<float>()
					};
					stabSweep.MiddlePoint = {
						i["stabsweep"]["middlepoint"][0].get<float>(),
						i["stabsweep"]["middlepoint"][1].get<float>(),
						i["stabsweep"]["middlepoint"][2].get<float>()
					};
					myBehaviours.SetValue("StabSweep" + std::to_string(number), stabSweep);
					//StabAndStuck
					stabAndStuck.StabDamage = i["stabandstuck"]["stabdamage"].get<float>();
					stabAndStuck.HeadDamage = i["stabandstuck"]["headdamage"].get<float>();
					stabAndStuck.AttackStayTime = i["stabandstuck"]["attackstaytime"].get<float>();
					stabAndStuck.StuckTime = i["stabandstuck"]["stucktime"].get<float>();
					stabAndStuck.ExtraDamage = i["stabandstuck"]["extradamage"].get<float>();
					stabAndStuck.StabForwarningTime = i["stabandstuck"]["stabforwarningtime"].get<float>();
					stabAndStuck.HeadForwarningTime = i["stabandstuck"]["headforwarningtime"].get<float>();
					stabAndStuck.SizePerStabAttack = i["stabandstuck"]["sizeperstabattack"].get<float>();
					stabAndStuck.SizePerHeadAttack = i["stabandstuck"]["sizeperheadattack"].get<float>();
					stabAndStuck.DelayBetweenStabs = i["stabandstuck"]["delaybetweenstabs"].get<float>();
					stabAndStuck.DelayBeforeHeadAttack = i["stabandstuck"]["delaybeforeheadattack"].get<float>();
					stabAndStuck.RandomStabPlacementRange = i["stabandstuck"]["randomstabplacementrange"].get<float>();
					stabAndStuck.StabCount = i["stabandstuck"]["stabcount"].get<int>();
					stabAndStuck.StabsLeft = stabAndStuck.StabCount;
					stabAndStuck.HeadHitPoint = {
						i["stabandstuck"]["headhitpoint"][0].get<float>(),
						i["stabandstuck"]["headhitpoint"][1].get<float>(),
						i["stabandstuck"]["headhitpoint"][2].get<float>()
					};
					myBehaviours.SetValue("StabAndStuck" + std::to_string(number), stabAndStuck);
					//RoarHelp
					roarHelp.DelayBetweenSpawns = i["roarhelp"]["delaybetweenspawns"].get<float>();
					roarHelp.SpawnPushToMiddle = i["roarhelp"]["spawnpushtomiddle"].get<float>();
					roarHelp.SpawnCountRangeLurker = {
						i["roarhelp"]["spawncountrangelurker"][0].get<int>(),
						i["roarhelp"]["spawncountrangelurker"][1].get<int>(),
					};
					roarHelp.SpawnCountRangeHunter = {
						i["roarhelp"]["spawncountrangehunter"][0].get<int>(),
						i["roarhelp"]["spawncountrangehunter"][1].get<int>(),
					};
					roarHelp.MiddlePoint = {
						i["roarhelp"]["middlepoint"][0].get<float>(),
						i["roarhelp"]["middlepoint"][1].get<float>(),
						i["roarhelp"]["middlepoint"][2].get<float>()
					};
					myBehaviours.SetValue("RoarHelp" + std::to_string(number), roarHelp);
					number++;
				}
			}
			stream.close();
			//Elites
			{
				std::ifstream eliteStream("../Assets/Json/EliteEnemyModifiers.json");
				nlohmann::json eliteJson;
				if (!eliteStream.good())
					return;

				eliteStream >> eliteJson;
				eliteStream.close();
				myBehaviours.SetValue("EliteLurkerHealth", eliteJson["Lurker"]["Health"].get<float>());
				myBehaviours.SetValue("EliteLurkerDamage", eliteJson["Lurker"]["Damage"].get<float>());
				myBehaviours.SetValue("EliteHunterHealth", eliteJson["Hunter"]["Health"].get<float>());
				myBehaviours.SetValue("EliteHunterDamage", eliteJson["Hunter"]["Damage"].get<float>());
			}
			//Player
			{
				std::ifstream statsStream("../Assets/Json/playerstats.json");
				nlohmann::json statsReader;

				if (!statsStream.good())
					return;

				statsStream >> statsReader;

				myBehaviours.SetValue("PlayerSpeed", statsReader["speed"].get<float>());
				myBehaviours.SetValue("PlayerHealth", statsReader["health"].get<float>());
				myBehaviours.SetValue("PlayerHealthreg", statsReader["healthreg"].get<float>());
				myBehaviours.SetValue("PlayerMana", statsReader["mana"].get<float>());
				myBehaviours.SetValue("PlayerManareg", statsReader["manareg"].get<float>());

				myBehaviours.SetValue("PlayerReqExp", statsReader["experienceRequiredToLevelUp"].get<int>());

				// Primary

				myBehaviours.SetValue("PlayerPriDamage", statsReader["primaryattack"]["damage"].get<float>());
				myBehaviours.SetValue("PlayerPriRange", statsReader["primaryattack"]["range"].get<float>());
				myBehaviours.SetValue("PlayerPriAttackspeed", statsReader["primaryattack"]["attackspeed"].get<float>());
				myBehaviours.SetValue("PlayerPriStayrate", statsReader["primaryattack"]["stayrate"].get<float>());

				//secondary

				myBehaviours.SetValue("PlayerSecDamage", statsReader["secondaryattack"]["damage"].get<float>());
				myBehaviours.SetValue("PlayerSecStay", statsReader["secondaryattack"]["stay"].get<float>());
				myBehaviours.SetValue("PlayerSecMoveSpeed", statsReader["secondaryattack"]["movementspeed"].get<float>());
				myBehaviours.SetValue("PlayerSecCost", statsReader["secondaryattack"]["cost"].get<float>());
				myBehaviours.SetValue("PlayerSecStalTime", statsReader["secondaryattack"]["staltime"].get<float>());

				// Magic Armor

				myBehaviours.SetValue("PlayerArmCooldown", statsReader["magicarmor"]["cooldown"].get<float>());
				myBehaviours.SetValue("PlayerArmCost", statsReader["magicarmor"]["cost"].get<float>());
				myBehaviours.SetValue("PlayerArmDuration", statsReader["magicarmor"]["duration"].get<float>());

				// Teleport ability

				myBehaviours.SetValue("PlayerTelCooldown", statsReader["teleport"]["cooldown"].get<float>());
				myBehaviours.SetValue("PlayerTelCost", statsReader["teleport"]["cost"].get<float>());
				myBehaviours.SetValue("PlayerTelDistance", statsReader["teleport"]["maxdistance"].get<float>());

				// AoEDoT

				myBehaviours.SetValue("PlayerAoeCooldown", statsReader["AoEDoT"]["cooldown"].get<float>());
				myBehaviours.SetValue("PlayerAoeCost", statsReader["AoEDoT"]["cost"].get<float>());
				myBehaviours.SetValue("PlayerAoeDamage", statsReader["AoEDoT"]["damage"].get<float>());
				myBehaviours.SetValue("PlayerAoeDuration", statsReader["AoEDoT"]["duration"].get<float>());
				myBehaviours.SetValue("PlayerAoeRadius", statsReader["AoEDoT"]["radius"].get<float>());

				// Ultimate

				myBehaviours.SetValue("PlayerUltCooldown", statsReader["ultimate"]["cooldown"].get<float>());
				myBehaviours.SetValue("PlayerUltCost", statsReader["ultimate"]["cost"].get<float>());
				myBehaviours.SetValue("PlayerUltDamage", statsReader["ultimate"]["damage"].get<float>());
				myBehaviours.SetValue("PlayerUltRadius", statsReader["ultimate"]["radius"].get<float>());
				myBehaviours.SetValue("PlayerUltRange", statsReader["ultimate"]["range"].get<float>());
				myBehaviours.SetValue("PlayerUltRegen", statsReader["ultimate"]["healthregen"].get<float>());
				statsStream.close();
			}

#pragma region Player Anim Blend
			std::ifstream blendReader("../Assets/Json/PlayerAnimBlendData.json");
			nlohmann::json blendData;
			if (!blendReader.good())
				return;
			blendReader >> blendData;
			blendReader.close();


			myBehaviours.SetValue<float>("PlayerMoveToIdleSpeed", blendData["MoveTo"]["Idle"]);
			myBehaviours.SetValue<float>("PlayerIdleToMoveSpeed", blendData["IdleTo"]["Move"]);

			myBehaviours.SetValue<float>("PlayerAnyToMoveSpeed", blendData["AnyTo"]["Move"]);
			myBehaviours.SetValue<float>("PlayerAnyToIdleSpeed", blendData["AnyTo"]["Idle"]);
			myBehaviours.SetValue<float>("PlayerAnyToPrimaryAttackSpeed", blendData["AnyTo"]["Primary"]);
			myBehaviours.SetValue<float>("PlayerAnyToSecondaryAttackSpeed", blendData["AnyTo"]["Secondary"]);
			myBehaviours.SetValue<float>("PlayerAnyToSpellSpeed", blendData["AnyTo"]["Spell"]);
			myBehaviours.SetValue<float>("PlayerAnyToDeadSpeed", blendData["AnyTo"]["Dead"]);
			myBehaviours.SetValue<float>("PlayerAnyToBagSpeed", blendData["AnyTo"]["Bag"]);

#pragma endregion
		}
	}


}
