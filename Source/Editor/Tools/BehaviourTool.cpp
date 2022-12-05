#include "BehaviourTool.h"
#include "ISTE/CU/Database.h"
#include "ISTE/Context.h"
#include "ISTE/Scene/SceneHandler.h"
#include "ISTE/Scene/Scene.h"
#include "imgui/imgui.h"
#include "imgui/misc/cpp/imgui_stdlib.h"
#include <fstream>
#include "Json/json.hpp"
#include "ISTE/ECSB/BobStructs.h"

#define END1 }
#define END2 }}
#define END3 }}}
#define END4 }}}}
#define END5 }}}}}
#define END6 }}}}}}
#define END7 }}}}}}}
#define END8 }}}}}}}}
#define END9 }}}}}}}}}


ISTE::BehaviourTool::~BehaviourTool()
{
}
void ISTE::BehaviourTool::Draw()
{
	if (!myActive)
	{
		return;
	}



	ImGui::Begin("Behaviour Tool", &myActive);
	ImGui::Combo("Behaviours", &mySelectedBehaviour, myBehaviours, IM_ARRAYSIZE(myBehaviours));
	ImGui::PushItemWidth(150.0f);
	ImGui::InputFloat("Drag Sensitivity", &myDragSensitivity);
	ImGui::PopItemWidth();
	if (myDragSensitivity < 0)
		myDragSensitivity = 0;
	switch (mySelectedBehaviour)
	{
	case 0:
	{
		HunterEdit();
		break;
	}
	case 1:
	{
		LurkerEdit();
		break;
	}
	case 2:
	{
		PlayerEdit();
		break;
	}
	case 3:
	{
		BossEdit();
		break;
	}
	case 4:
	{
		EliteEdit();
		break;
	}
	case 5:
	{
		IdleEdit();
	}
	default:
	{
		break;
	}
	}
	if (ImGui::Button("Set"))
	{
		Context::Get()->mySceneHandler->GetActiveScene().GetUpdateBehaviours() = true;
	}
	ImGui::SameLine();
	if (ImGui::Button("Save"))
	{
		SaveBehaviours();
	}
	ImGui::SameLine();
	if (ImGui::Button("Load"))
	{
		Context::Get()->mySceneHandler->GetActiveScene().LoadBehaviours();
	}
	ImGui::End();

}

void ISTE::BehaviourTool::Init(BaseEditor*)
{
	myToolName = "Behaviour Tool";
}
void ISTE::BehaviourTool::IdleEdit()
{
	CU::Database<true>& behaviourDB = ISTE::Context::Get()->mySceneHandler->GetActiveScene().GetBehaviourDatabase();
	float& Radius = behaviourDB.Get<float>("IdleRadius");
	ImGui::PushItemWidth(150.0f);
	ImGui::Text("Idle");
	ImGui::DragFloat("Radius: ##1", &Radius, myDragSensitivity);
	float& speed = behaviourDB.Get<float>("IdleSpeed");
	ImGui::DragFloat("Speed: ##1", &speed, myDragSensitivity);
	float& rest = behaviourDB.Get<float>("IdleRest");
	ImGui::DragFloat("Rest: ##1", &rest, myDragSensitivity);
	float& damageDuration = behaviourDB.Get<float>("IdleDamageDuration");
	ImGui::DragFloat("Damage Duration: ##1", &damageDuration, myDragSensitivity);
	float& health = behaviourDB.Get<float>("IdleHealth");
	ImGui::DragFloat("Health: ##1", &health, myDragSensitivity);
	ImGui::PopItemWidth();
}
void ISTE::BehaviourTool::LurkerEdit()
{
	CU::Database<true>& behaviourDB = ISTE::Context::Get()->mySceneHandler->GetActiveScene().GetBehaviourDatabase();
	float& speed = behaviourDB.Get<float>("LurkerSpeed");
	ImGui::PushItemWidth(150.0f);
	ImGui::Text("Lurker");
	ImGui::DragFloat("Speed: ##1", &speed, myDragSensitivity);
	float& findRadius = behaviourDB.Get<float>("LurkerFindRadius");
	ImGui::DragFloat("Find Radius: ##1", &findRadius, myDragSensitivity);
	float& maxTimer = behaviourDB.Get<float>("LurkerWatchTime");
	ImGui::DragFloat("Watch Time: ##1", &maxTimer, myDragSensitivity);
	float& keepDistance = behaviourDB.Get<float>("LurkerKeepDistance");
	ImGui::DragFloat("Keep Distance: ##1", &keepDistance, myDragSensitivity);
	float& health = behaviourDB.Get<float>("LurkerHealth");
	ImGui::DragFloat("Health: ##1", &health, myDragSensitivity);
	ImGui::Separator();
	ImGui::Text("Weapon");
	float& damage = behaviourDB.Get<float>("LurkerWeaponDamage");
	ImGui::DragFloat("Damage: ##2", &damage, myDragSensitivity);
	float& attackSize = behaviourDB.Get<float>("LurkerWeaponSize");
	ImGui::DragFloat("Size: ##2", &attackSize, myDragSensitivity);
	float& attackSpeed = behaviourDB.Get<float>("LurkerWeaponAttackSpeed");
	ImGui::DragFloat("Speed: ##2", &attackSpeed, myDragSensitivity);
	float& stayRate = behaviourDB.Get<float>("LurkerWeaponStay");
	ImGui::DragFloat("Stay: ##2", &stayRate, myDragSensitivity);
	ImGui::Separator();
	std::string& name = behaviourDB.Get<std::string>("LurkerName");
	ImGui::InputText("Name: ", &name, myDragSensitivity);

	int& experience = behaviourDB.Get<int>("LurkerExperience");
	ImGui::DragInt("Experience: ", &experience, myDragSensitivity, 0);
	ImGui::PopItemWidth();
}
void ISTE::BehaviourTool::HunterEdit()
{
	CU::Database<true>& behaviourDB = ISTE::Context::Get()->mySceneHandler->GetActiveScene().GetBehaviourDatabase();
	float& speed = behaviourDB.Get<float>("HunterSpeed");
	ImGui::PushItemWidth(150.0f);
	ImGui::Text("Hunter");
	ImGui::DragFloat("Speed: ##1", &speed, myDragSensitivity);
	float& findRadius = behaviourDB.Get<float>("HunterFindRadius");
	ImGui::DragFloat("Find Radius: ##1", &findRadius, myDragSensitivity);
	float& maxTimer = behaviourDB.Get<float>("HunterWatchTime");
	ImGui::DragFloat("Watch Time: ##1", &maxTimer, myDragSensitivity);
	float& keepDistance = behaviourDB.Get<float>("HunterKeepDistance");
	ImGui::DragFloat("Keep Distance: ##1", &keepDistance, myDragSensitivity);
	float& health = behaviourDB.Get<float>("HunterHealth");
	ImGui::DragFloat("Health: ##1", &health, myDragSensitivity);
	ImGui::Separator();
	ImGui::Text("Explosion");
	float& suicideDistance = behaviourDB.Get<float>("HunterExplosionDistance");
	ImGui::DragFloat("Distance: ##2", &suicideDistance, myDragSensitivity);
	float& warningDelay = behaviourDB.Get<float>("HunterWarningDelay");
	ImGui::DragFloat("Delay: ##2", &warningDelay, myDragSensitivity);
	float& explosionRange = behaviourDB.Get<float>("HunterExplosionRange");
	ImGui::DragFloat("Range: ##2", &explosionRange, myDragSensitivity);
	float& explosionDamage = behaviourDB.Get<float>("HunterExplosionDamage");
	ImGui::DragFloat("Damage: ##2", &explosionDamage, myDragSensitivity);
	ImGui::Separator();
	ImGui::Text("Weapon");
	float& damage = behaviourDB.Get<float>("HunterWeaponDamage");
	ImGui::DragFloat("Damage: ##3", &damage, myDragSensitivity);
	float& attackSize = behaviourDB.Get<float>("HunterWeaponSize");
	ImGui::DragFloat("Size: ##3", &attackSize, myDragSensitivity);
	float& attackSpeed = behaviourDB.Get<float>("HunterWeaponAttackSpeed");
	ImGui::DragFloat("Attack Speed: ##3", &attackSpeed, myDragSensitivity);
	float& stayRate = behaviourDB.Get<float>("HunterWeaponStay");
	ImGui::DragFloat("Stay: ##3", &stayRate, myDragSensitivity);
	float& weaponSpeed = behaviourDB.Get<float>("HunterWeaponSpeed");
	ImGui::DragFloat("Speed: ##3", &weaponSpeed, myDragSensitivity);
	float& minimumDistance = behaviourDB.Get<float>("HunterWeaponMinDistance");
	ImGui::DragFloat("Minimum Distance: ##3", &minimumDistance, myDragSensitivity);
	ImGui::Separator();
	std::string& name = behaviourDB.Get<std::string>("HunterName");
	ImGui::InputText("Hunter Name: ", &name, myDragSensitivity);

	int& experience = behaviourDB.Get<int>("HunterExperience");
	ImGui::DragInt("Hunter Experience: ", &experience, myDragSensitivity, 0);
	ImGui::PopItemWidth();
}
void ISTE::BehaviourTool::BossEdit()
{
	ImGui::PushItemWidth(150.0f);
	CU::Database<true>& behaviours = ISTE::Context::Get()->mySceneHandler->GetActiveScene().GetBehaviourDatabase();
	float& bossHealth = behaviours.Get<float>("BossHealth");
	float& bossDamageShaderDuration = behaviours.Get<float>("BossDamageDuration");
	std::string& bossName = behaviours.Get<std::string>("BossName");
	float& bossRotationSpeed = behaviours.Get<float>("BossRotationSpeed");
	ImGui::DragFloat("Health: ", &bossHealth, myDragSensitivity);
	ImGui::DragFloat("Damage Shader Duration: ", &bossDamageShaderDuration, myDragSensitivity);
	ImGui::DragFloat("Rotation Speed: ", &bossRotationSpeed, myDragSensitivity);
	ImGui::InputText("Name: ", &bossName);
	ImGui::Separator();

	ImGui::Combo("Boss Stage", &mySelectedBossStage, myBossStages, IM_ARRAYSIZE(myBossStages));
	{


		std::vector<int>* existenceCheck;
		if (!behaviours.TryGet<std::vector<int>*>("AttackOptions" + std::to_string(mySelectedBossStage), existenceCheck))
		{
			ImGui::PopItemWidth();
			return;
		}
		std::vector<int>* attackOptions = behaviours.Get<std::vector<int>*>("AttackOptions" + std::to_string(mySelectedBossStage));
		ImGui::Text("Attack Options");
		if (ImGui::Selectable("Add"))
		{
			attackOptions->push_back(int(0));
		}
		if (ImGui::Selectable("Remove") && attackOptions->size() > 0)
		{
			attackOptions->pop_back();
		}
		for (int j = 0; j < attackOptions->size(); j++)
		{
			std::string number = std::to_string(j);
			int test = (*attackOptions)[j];
			ImGui::DragInt(("Attack Option: ##" + number).c_str(), &(*attackOptions)[j], 1.0f, 1, 4);
		}

		AcidSpit& acidSpit = behaviours.Get<AcidSpit>("AcidSpit" + std::to_string(mySelectedBossStage));
		StabSweep& stabSweep = behaviours.Get<StabSweep>("StabSweep" + std::to_string(mySelectedBossStage));
		StabAndStuck& stabAndStuck = behaviours.Get<StabAndStuck>("StabAndStuck" + std::to_string(mySelectedBossStage));
		RoarHelp& roarHelp = behaviours.Get<RoarHelp>("RoarHelp" + std::to_string(mySelectedBossStage));

		float& orderDelay = behaviours.Get<float>("orderdelay" + std::to_string(mySelectedBossStage));
		ImGui::DragFloat("Order Delay: ", &orderDelay);
		ImGui::Separator();

		//AcidSpit
		ImGui::Text("Acid Spit");
		ImGui::DragFloat("Damage: ##1", &acidSpit.Damage, myDragSensitivity);
		ImGui::DragFloat("Stay Time: ##1", &acidSpit.StayTime, myDragSensitivity);
		ImGui::DragFloat("Prewarning Time: ##1", &acidSpit.PrewarningTime, myDragSensitivity);
		ImGui::DragFloat("Size Per Puddle: ##1", &acidSpit.SizePerPuddle, myDragSensitivity);
		ImGui::DragFloat("Size Per Spit: ##1", &acidSpit.SizePerSpit, myDragSensitivity);
		ImGui::DragFloat("Random Placement Range: ##1", &acidSpit.RandomAcidSpitPlacementRange, myDragSensitivity);
		ImGui::DragFloat("Delay Between Spit: ##1", &acidSpit.DelayBetweenSpits, myDragSensitivity);
		ImGui::DragFloat("Delay After All Shots: ##1", &acidSpit.DelayAfterAllShots, myDragSensitivity);
		ImGui::DragFloat("Extra Height: ##1", &acidSpit.ExtraHeight, myDragSensitivity);
		ImGui::DragInt("Spit Count: ##1", &acidSpit.SpitCount, myDragSensitivity);
		ImGui::DragFloat3("Spit Count: ##1", acidSpit.AttackSource.myValue, myDragSensitivity);
		ImGui::Separator();
		//StabSweep
		ImGui::Text("Stab Sweep");
		ImGui::DragFloat("Damage: ##2", &stabSweep.Damage, myDragSensitivity);
		ImGui::DragFloat("Speed: ##2", &stabSweep.Speed, myDragSensitivity);
		ImGui::DragFloat("Size On Sweep: ##2", &stabSweep.SizeOnSweep, myDragSensitivity);
		ImGui::DragFloat("Warning Time: ##2", &stabSweep.ForwarningTime, myDragSensitivity);
		ImGui::DragFloat3("Left Point: ##2", stabSweep.LeftPoint.myValue, myDragSensitivity);
		ImGui::DragFloat3("Middle Point: ##2", stabSweep.MiddlePoint.myValue, myDragSensitivity);
		ImGui::DragFloat3("Right Point: ##2", stabSweep.RightPoint.myValue, myDragSensitivity);
		ImGui::Separator();
		//StabAndStuck
		ImGui::Text("Stab And Stuck");
		ImGui::DragFloat("Stab Damage: ##3", &stabAndStuck.StabDamage, myDragSensitivity);
		ImGui::DragFloat("Head Damage: ##3", &stabAndStuck.HeadDamage, myDragSensitivity);
		ImGui::DragFloat("Attack Stay Time: ##3", &stabAndStuck.AttackStayTime, myDragSensitivity);
		ImGui::DragFloat("Stuck Time: ##3", &stabAndStuck.StuckTime, myDragSensitivity);
		ImGui::DragFloat("Extra Damage: ##3", &stabAndStuck.ExtraDamage, myDragSensitivity);
		ImGui::DragFloat("Stab Warning Time: ##3", &stabAndStuck.StabForwarningTime, myDragSensitivity);
		ImGui::DragFloat("Head Warning Time: ##3", &stabAndStuck.HeadForwarningTime, myDragSensitivity);
		ImGui::DragFloat("Size Per Stab: ##3", &stabAndStuck.SizePerStabAttack, myDragSensitivity);
		ImGui::DragFloat("Size Per Head: ##3", &stabAndStuck.SizePerHeadAttack, myDragSensitivity);
		ImGui::DragFloat("Delay Between Stabs: ##3", &stabAndStuck.DelayBetweenStabs, myDragSensitivity);
		ImGui::DragFloat("Delay Before Head: ##3", &stabAndStuck.DelayBeforeHeadAttack, myDragSensitivity);
		ImGui::DragFloat("Random Stab Range: ##3", &stabAndStuck.RandomStabPlacementRange, myDragSensitivity);
		ImGui::DragInt("Stab Count: ##3", &stabAndStuck.StabCount, myDragSensitivity);
		ImGui::DragFloat3("Head Hit Point: ##3", stabAndStuck.HeadHitPoint.myValue, myDragSensitivity);
		ImGui::Separator();
		//RoarHelp
		ImGui::Text("Roar Help");
		ImGui::DragFloat("Delay Between Spawns: ##4", &roarHelp.DelayBetweenSpawns, myDragSensitivity);
		ImGui::DragFloat("Spawn Push To Middle: ##4", &roarHelp.SpawnPushToMiddle, myDragSensitivity);
		ImGui::DragInt2("Spawn Count Range Lurker: ##4", roarHelp.SpawnCountRangeLurker.myValue, myDragSensitivity);
		ImGui::DragInt2("Spawn Count Range Hunter: ##4", roarHelp.SpawnCountRangeHunter.myValue, myDragSensitivity);
		ImGui::DragFloat3("Middlepoint: ##4", roarHelp.MiddlePoint.myValue, myDragSensitivity);
		ImGui::Separator();
	}
	ImGui::PopItemWidth();
}
void ISTE::BehaviourTool::PlayerEdit()
{
	CU::Database<true>& behaviours = ISTE::Context::Get()->mySceneHandler->GetActiveScene().GetBehaviourDatabase();
	ImGui::PushItemWidth(150.0f);
	ImGui::Text("Player");
	float& speed = behaviours.Get<float>("PlayerSpeed");
	ImGui::DragFloat("Speed: ##1", &speed, myDragSensitivity);
	float& health = behaviours.Get<float>("PlayerHealth");
	ImGui::DragFloat("Health: ##1", &health, myDragSensitivity);
	float& healthRegen = behaviours.Get<float>("PlayerHealthreg");
	ImGui::DragFloat("Health Regen: ##1", &healthRegen, myDragSensitivity);
	float& mana = behaviours.Get<float>("PlayerMana");
	ImGui::DragFloat("Mana: ##1", &mana, myDragSensitivity);
	float& manaRegen = behaviours.Get<float>("PlayerManareg");
	ImGui::DragFloat("Mana Regen: ##1", &manaRegen, myDragSensitivity);

	int& expReq = behaviours.Get<int>("PlayerReqExp");
	ImGui::DragInt("Exp Required: ", &expReq, myDragSensitivity, 0);

	// Primary
	ImGui::Separator();
	ImGui::Text("Primary");
	float& primaryDamage = behaviours.Get<float>("PlayerPriDamage");
	ImGui::DragFloat("Damage: ##2", &primaryDamage, myDragSensitivity);
	float& primaryRange = behaviours.Get<float>("PlayerPriRange");
	ImGui::DragFloat("Range: ##2", &primaryRange, myDragSensitivity);
	float& primaryAttackSpeed = behaviours.Get<float>("PlayerPriAttackspeed");
	ImGui::DragFloat("Attack Speed: ##2", &primaryAttackSpeed, myDragSensitivity);
	float& primaryStayrate = behaviours.Get<float>("PlayerPriStayrate");
	ImGui::DragFloat("Stayrate: ##2", &primaryStayrate, myDragSensitivity);


	//secondary
	ImGui::Separator();
	ImGui::Text("Secondary");
	float& secondaryDamage = behaviours.Get<float>("PlayerSecDamage");
	ImGui::DragFloat("Damage: ##3", &secondaryDamage, myDragSensitivity);
	float& secondaryStay = behaviours.Get<float>("PlayerSecStay");
	ImGui::DragFloat("Stay: ##3", &secondaryStay, myDragSensitivity);
	float& secondarySpeed = behaviours.Get<float>("PlayerSecMoveSpeed");
	ImGui::DragFloat("Speed: ##3", &secondarySpeed, myDragSensitivity);
	float& secondaryCost = behaviours.Get<float>("PlayerSecCost");
	ImGui::DragFloat("Cost: ##3", &secondaryCost, myDragSensitivity);
	float& secondaryStal = behaviours.Get<float>("PlayerSecStalTime");
	ImGui::DragFloat("Stalling Time: ##3", &secondaryStal, myDragSensitivity);

	// Magic Armor
	ImGui::Separator();
	ImGui::Text("Armor");
	float& armorCooldown = behaviours.Get<float>("PlayerArmCooldown");
	ImGui::DragFloat("Cooldown: ##4", &armorCooldown, myDragSensitivity);
	float& armorCost = behaviours.Get<float>("PlayerArmCost");
	ImGui::DragFloat("Cost: ##4", &armorCost, myDragSensitivity);
	float& armorDuration = behaviours.Get<float>("PlayerArmDuration");
	ImGui::DragFloat("Duration: ##4", &armorDuration, myDragSensitivity);

	// Teleport ability
	ImGui::Separator();
	ImGui::Text("Teleport");

	float& teleCooldown = behaviours.Get<float>("PlayerTelCooldown");
	ImGui::DragFloat("Cooldown: ##5", &teleCooldown, myDragSensitivity);
	float& teleCost = behaviours.Get<float>("PlayerTelCost");
	ImGui::DragFloat("Cost: ##5", &teleCost, myDragSensitivity);
	float& teleDistance = behaviours.Get<float>("PlayerTelDistance");
	ImGui::DragFloat("Distance: ##5", &teleDistance, myDragSensitivity);

	// AoEDoT
	ImGui::Separator();
	ImGui::Text("Area of Effect");
	float& aoeCooldown = behaviours.Get<float>("PlayerAoeCooldown");
	ImGui::DragFloat("Cooldown: ##6", &aoeCooldown, myDragSensitivity);
	float& aoeCost = behaviours.Get<float>("PlayerAoeCost");
	ImGui::DragFloat("Cost: ##6", &aoeCost, myDragSensitivity);
	float& aoeDamage = behaviours.Get<float>("PlayerAoeDamage");
	ImGui::DragFloat("Damage: ##6", &aoeDamage, myDragSensitivity);
	float& aoeDuration = behaviours.Get<float>("PlayerAoeDuration");
	ImGui::DragFloat("Duration: ##6", &aoeDuration, myDragSensitivity);
	float& aoeRadius = behaviours.Get<float>("PlayerAoeRadius");
	ImGui::DragFloat("Radius: ##6", &aoeRadius, myDragSensitivity);

	// Ultimate
	ImGui::Separator();
	ImGui::Text("Ultimate");
	float& ultCooldown = behaviours.Get<float>("PlayerUltCooldown");
	ImGui::DragFloat("Cooldown: ##7", &ultCooldown, myDragSensitivity);
	float& ultCost = behaviours.Get<float>("PlayerUltCost");
	ImGui::DragFloat("Cost: ##7", &ultCost, myDragSensitivity);
	float& ultDamage = behaviours.Get<float>("PlayerUltDamage");
	ImGui::DragFloat("Damage: ##7", &ultDamage, myDragSensitivity);
	float& ultRadius = behaviours.Get<float>("PlayerUltRadius");
	ImGui::DragFloat("Radius: ##7", &ultRadius, myDragSensitivity);
	float& ultRange = behaviours.Get<float>("PlayerUltRange");
	ImGui::DragFloat("Range: ##7", &ultRange, myDragSensitivity);
	float& ultRegen = behaviours.Get<float>("PlayerUltRegen");
	ImGui::DragFloat("Regen: ##7", &ultRegen, myDragSensitivity);
	ImGui::PopItemWidth();
}
void ISTE::BehaviourTool::EliteEdit()
{
	CU::Database<true>& behaviourDB = ISTE::Context::Get()->mySceneHandler->GetActiveScene().GetBehaviourDatabase();
	ImGui::PushItemWidth(150.0f);
	ImGui::Text("Hunter");
	float& hunterDamage = behaviourDB.Get<float>("EliteHunterDamage");
	ImGui::DragFloat("Damage: ##1", &hunterDamage, myDragSensitivity);
	float& hunterHealth = behaviourDB.Get<float>("EliteHunterHealth");
	ImGui::DragFloat("Health: ##1", &hunterHealth, myDragSensitivity);
	ImGui::Separator();
	ImGui::Text("Lurker");
	float& lurkerDamage = behaviourDB.Get<float>("EliteLurkerDamage");
	ImGui::DragFloat("Damage: ##2", &lurkerDamage, myDragSensitivity);
	float& lurkerHealth = behaviourDB.Get<float>("EliteLurkerHealth");
	ImGui::DragFloat("Health: ##2", &lurkerHealth, myDragSensitivity);
	ImGui::PopItemWidth();
}
void ISTE::BehaviourTool::SaveBehaviours()
{
	Scene& scene = ISTE::Context::Get()->mySceneHandler->GetActiveScene();
	CU::Database<true>& behaviours = scene.GetBehaviourDatabase();
	{// JSON Export
		std::ifstream stream("../Assets/Json/enemybehaviours.json");
		if (stream)
		{
			nlohmann::json reader;
			stream >> reader;
			stream.close();
			// Hunter
			reader["hunter"]["speed"] = behaviours.Get<float>("HunterSpeed");
			reader["hunter"]["findradius"] = behaviours.Get<float>("HunterFindRadius");
			reader["hunter"]["watchtime"] = behaviours.Get<float>("HunterWatchTime");
			reader["hunter"]["keepdistance"] = behaviours.Get<float>("HunterKeepDistance");
			reader["hunter"]["health"] = behaviours.Get<float>("HunterHealth");

			reader["hunter"]["explosionstats"]["explosiondistance"] = behaviours.Get<float>("HunterExplosionDistance");
			reader["hunter"]["explosionstats"]["warningdelay"] = behaviours.Get<float>("HunterWarningDelay");
			reader["hunter"]["explosionstats"]["explosionrange"] = behaviours.Get<float>("HunterExplosionRange");
			reader["hunter"]["explosionstats"]["explosiondamage"] = behaviours.Get<float>("HunterExplosionDamage");

			reader["hunter"]["weapon"]["damage"] = behaviours.Get<float>("HunterWeaponDamage");
			reader["hunter"]["weapon"]["size"] = behaviours.Get<float>("HunterWeaponSize");
			reader["hunter"]["weapon"]["attackspeed"] = behaviours.Get<float>("HunterWeaponAttackSpeed");
			reader["hunter"]["weapon"]["stayrate"] = behaviours.Get<float>("HunterWeaponStay");
			reader["hunter"]["weapon"]["Speed"] = behaviours.Get<float>("HunterWeaponSpeed");
			reader["hunter"]["weapon"]["MinimumDistance"] = behaviours.Get<float>("HunterWeaponMinDistance");

			reader["hunter"]["name"] = behaviours.Get<std::string>("HunterName");
			reader["hunter"]["experience"] = behaviours.Get<int>("HunterExperience");

			// Lurker
			reader["lurker"]["speed"] = behaviours.Get<float>("LurkerSpeed");
			reader["lurker"]["findradius"] = behaviours.Get<float>("LurkerFindRadius");
			reader["lurker"]["watchtime"] = behaviours.Get<float>("LurkerWatchTime");
			reader["lurker"]["keepdistance"] = behaviours.Get<float>("LurkerKeepDistance");
			reader["lurker"]["health"] = behaviours.Get<float>("LurkerHealth");

			reader["lurker"]["weapon"]["damage"] = behaviours.Get<float>("LurkerWeaponDamage");
			reader["lurker"]["weapon"]["size"] = behaviours.Get<float>("LurkerWeaponSize");
			reader["lurker"]["weapon"]["attackspeed"] = behaviours.Get<float>("LurkerWeaponAttackSpeed");
			reader["lurker"]["weapon"]["stayrate"] = behaviours.Get<float>("LurkerWeaponStay");

			reader["lurker"]["name"] = behaviours.Get<std::string>("LurkerName");
			reader["lurker"]["experience"] = behaviours.Get<int>("LurkerExperience");

			reader["bossbob"]["health"] = behaviours.Get<float>("BossHealth");
			reader["bossbob"]["damageshaderduration"] = behaviours.Get<float>("BossDamageDuration");
			reader["bossbob"]["name"] = behaviours.Get<std::string>("BossName");
			reader["bossbob"]["rotationspeed"] = behaviours.Get<float>("BossRotationSpeed");

			{
				//Boss bob
				int i = 0;
				for (auto& bossBehaviourJson : reader["bossbob"]["stages"].items())
				{
					std::vector<int>* attackOptions = behaviours.Get<std::vector<int>*>("AttackOptions" + std::to_string(i));
					AcidSpit& acidSpit = behaviours.Get<AcidSpit>("AcidSpit" + std::to_string(i));
					StabSweep& stabSweep = behaviours.Get<StabSweep>("StabSweep" + std::to_string(i));
					StabAndStuck& stabAndStuck = behaviours.Get<StabAndStuck>("StabAndStuck" + std::to_string(i));
					RoarHelp& roarHelp = behaviours.Get<RoarHelp>("RoarHelp" + std::to_string(i));
					nlohmann::json attackOptionJson;
					for (int j = 0; j < attackOptions->size(); j++)
					{
						attackOptionJson["attackoptions"].push_back((*attackOptions)[j]);
					}
					bossBehaviourJson.value() = attackOptionJson;
					//= attackOptionJson;
					bossBehaviourJson.value()["orderdelay"] = behaviours.Get<float>("orderdelay" + std::to_string(i));
					//AcidSpit
					bossBehaviourJson.value()["acidspit"]["damage"] = acidSpit.Damage;
					bossBehaviourJson.value()["acidspit"]["staytime"] = acidSpit.StayTime;
					bossBehaviourJson.value()["acidspit"]["prewarningtime"] = acidSpit.PrewarningTime;
					bossBehaviourJson.value()["acidspit"]["sizeperpuddle"] = acidSpit.SizePerPuddle;
					bossBehaviourJson.value()["acidspit"]["sizeperspit"] = acidSpit.SizePerSpit;
					bossBehaviourJson.value()["acidspit"]["randomacidspitplacementrange"] = acidSpit.RandomAcidSpitPlacementRange;
					bossBehaviourJson.value()["acidspit"]["delaybetweenspits"] = acidSpit.DelayBetweenSpits;
					bossBehaviourJson.value()["acidspit"]["delayafterallshots"] = acidSpit.DelayAfterAllShots;
					bossBehaviourJson.value()["acidspit"]["extraheight"] = acidSpit.ExtraHeight;
					bossBehaviourJson.value()["acidspit"]["spitcount"] = acidSpit.SpitCount;
					for (int k = 0; k < 3; k++)
					{
						bossBehaviourJson.value()["acidspit"]["attacksource"][k] = acidSpit.AttackSource.myValue[k];
					}
					//StabSweep
					bossBehaviourJson.value()["stabsweep"]["damage"] = stabSweep.Damage;
					bossBehaviourJson.value()["stabsweep"]["speed"] = stabSweep.Speed;
					bossBehaviourJson.value()["stabsweep"]["sizeonsweep"] = stabSweep.SizeOnSweep;
					bossBehaviourJson.value()["stabsweep"]["forwarningtime"] = stabSweep.ForwarningTime;
					for (int k = 0; k < 3; k++) // Seperated just to differentiate.
					{
						bossBehaviourJson.value()["stabsweep"]["leftpoint"][k] = stabSweep.LeftPoint.myValue[k];
					}
					for (int k = 0; k < 3; k++)
					{
						bossBehaviourJson.value()["stabsweep"]["rightpoint"][k] = stabSweep.RightPoint.myValue[k];
					}
					for (int k = 0; k < 3; k++)
					{
						bossBehaviourJson.value()["stabsweep"]["middlepoint"][k] = stabSweep.MiddlePoint.myValue[k];
					}
					for (int k = 0; k < 3; k++)
					{
						bossBehaviourJson.value()["stabsweep"]["attacksource"][k] = stabSweep.MiddlePoint.myValue[k];
					}
					//StabAndStuck
					bossBehaviourJson.value()["stabandstuck"]["stabdamage"] = stabAndStuck.StabDamage;
					bossBehaviourJson.value()["stabandstuck"]["headdamage"] = stabAndStuck.HeadDamage;
					bossBehaviourJson.value()["stabandstuck"]["attackstaytime"] = stabAndStuck.AttackStayTime;
					bossBehaviourJson.value()["stabandstuck"]["stucktime"] = stabAndStuck.StuckTime;
					bossBehaviourJson.value()["stabandstuck"]["extradamage"] = stabAndStuck.ExtraDamage;
					bossBehaviourJson.value()["stabandstuck"]["stabforwarningtime"] = stabAndStuck.StabForwarningTime;
					bossBehaviourJson.value()["stabandstuck"]["headforwarningtime"] = stabAndStuck.HeadForwarningTime;
					bossBehaviourJson.value()["stabandstuck"]["sizeperstabattack"] = stabAndStuck.SizePerStabAttack;
					bossBehaviourJson.value()["stabandstuck"]["sizeperheadattack"] = stabAndStuck.SizePerHeadAttack;
					bossBehaviourJson.value()["stabandstuck"]["delaybetweenstabs"] = stabAndStuck.DelayBetweenStabs;
					bossBehaviourJson.value()["stabandstuck"]["delaybeforeheadattack"] = stabAndStuck.DelayBeforeHeadAttack;
					bossBehaviourJson.value()["stabandstuck"]["randomstabplacementrange"] = stabAndStuck.RandomStabPlacementRange;
					bossBehaviourJson.value()["stabandstuck"]["stabcount"] = stabAndStuck.StabCount;
					for (int k = 0; k < 3; k++)
					{
						bossBehaviourJson.value()["stabandstuck"]["headhitpoint"][k] = stabAndStuck.HeadHitPoint.myValue[k];
					}
					//RoarHelp
					bossBehaviourJson.value()["roarhelp"]["delaybetweenspawns"] = roarHelp.DelayBetweenSpawns;
					bossBehaviourJson.value()["roarhelp"]["spawnpushtomiddle"] = roarHelp.SpawnPushToMiddle;
					for (int k = 0; k < 2; k++)
					{
						bossBehaviourJson.value()["roarhelp"]["spawncountrangelurker"][k] = roarHelp.SpawnCountRangeLurker.myValue[k];
					}
					for (int k = 0; k < 2; k++)
					{
						bossBehaviourJson.value()["roarhelp"]["spawncountrangehunter"][k] = roarHelp.SpawnCountRangeHunter.myValue[k];
					}
					for (int k = 0; k < 3; k++)
					{
						bossBehaviourJson.value()["roarhelp"]["middlepoint"][k] = roarHelp.MiddlePoint.myValue[k];
					}
					i++;
				}
			}

			std::ofstream streamOutput("../Assets/Json/enemybehaviours.json");
			streamOutput << reader;
			streamOutput.close();
			//Player
			std::ifstream statsStream("../Assets/Json/playerstats.json");
			nlohmann::json statsReader;

			if (!statsStream.good())
				return;

			statsStream >> statsReader;
			statsStream.close();
			statsReader["speed"] = behaviours.Get<float>("PlayerSpeed");
			statsReader["health"] = behaviours.Get<float>("PlayerHealth");
			statsReader["healthreg"] = behaviours.Get<float>("PlayerHealthreg");
			statsReader["mana"] = behaviours.Get<float>("PlayerMana");
			statsReader["manareg"] = behaviours.Get<float>("PlayerManareg");

			const int temp = behaviours.Get<int>("PlayerReqExp");
			statsReader["experienceRequiredToLevelUp"] = temp;

			// Primary

			statsReader["primaryattack"]["damage"] = behaviours.Get<float>("PlayerPriDamage");
			statsReader["primaryattack"]["range"] = behaviours.Get<float>("PlayerPriRange");
			statsReader["primaryattack"]["attackspeed"] = behaviours.Get<float>("PlayerPriAttackspeed");
			statsReader["primaryattack"]["stayrate"] = behaviours.Get<float>("PlayerPriStayrate");


			//secondary

			statsReader["secondaryattack"]["damage"] = behaviours.Get<float>("PlayerSecDamage");
			statsReader["secondaryattack"]["stay"] = behaviours.Get<float>("PlayerSecStay");
			statsReader["secondaryattack"]["movementspeed"] = behaviours.Get<float>("PlayerSecMoveSpeed");
			statsReader["secondaryattack"]["cost"] = behaviours.Get<float>("PlayerSecCost");
			statsReader["secondaryattack"]["staltime"] = behaviours.Get<float>("PlayerSecStalTime");

			// Magic Armor

			statsReader["magicarmor"]["cooldown"] = behaviours.Get<float>("PlayerArmCooldown");
			statsReader["magicarmor"]["cost"] = behaviours.Get<float>("PlayerArmCost");
			statsReader["magicarmor"]["duration"] = behaviours.Get<float>("PlayerArmDuration");

			// Teleport ability

			statsReader["teleport"]["cost"] = behaviours.Get<float>("PlayerTelCost");
			statsReader["teleport"]["maxdistance"] = behaviours.Get<float>("PlayerTelDistance");

			// AoEDoT

			statsReader["AoEDoT"]["cooldown"] = behaviours.Get<float>("PlayerAoeCooldown");
			statsReader["AoEDoT"]["cost"] = behaviours.Get<float>("PlayerAoeCost");
			statsReader["AoEDoT"]["damage"] = behaviours.Get<float>("PlayerAoeDamage");
			statsReader["AoEDoT"]["duration"] = behaviours.Get<float>("PlayerAoeDuration");
			statsReader["AoEDoT"]["radius"] = behaviours.Get<float>("PlayerAoeRadius");

			// Ultimate

			statsReader["ultimate"]["cooldown"] = behaviours.Get<float>("PlayerUltCooldown");
			statsReader["ultimate"]["cost"] = behaviours.Get<float>("PlayerUltCost");
			statsReader["ultimate"]["damage"] = behaviours.Get<float>("PlayerUltDamage");
			statsReader["ultimate"]["radius"] = behaviours.Get<float>("PlayerUltRadius");
			statsReader["ultimate"]["range"] = behaviours.Get<float>("PlayerUltRange");
			statsReader["ultimate"]["healthregen"] = behaviours.Get<float>("PlayerUltRegen");


			std::ofstream playerStreamOutput("../Assets/Json/playerstats.json");
			playerStreamOutput << statsReader;
			playerStreamOutput.close();

		}


	}
}