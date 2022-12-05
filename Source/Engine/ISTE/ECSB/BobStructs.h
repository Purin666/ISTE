#pragma once
#include "Behaviour.h"
#include "ISTE/Math/Vec3.h"
#include "ISTE/Math/Vec2.h"
#include <string>
#include <vector>
#include <map>
namespace ISTE
{
	struct AcidSpit
	{
		float Damage = 0.f;
		float StayTime = 0.f;
		float PrewarningTime = 0.f;
		float SizePerPuddle = 0.f;
		float SizePerSpit = 0.f;
		float RandomAcidSpitPlacementRange = 0.f;
		float DelayBetweenSpits = 0.f;
		float DelayAfterAllShots = 0.f;
		float ExtraHeight = 0.f;

		bool ShotFired = false;
		bool AcidOnGround = false;
		bool ReadyToSpit = false;
		bool DoingAnimation = false;

		int SpitCount = 0;
		int SpitsLeft = 0;

		int SpitIdCount = 0;
		int AcidIdCount = 0;

		CU::Vec3f AttackSource;
	};
	struct StabSweep
	{
		float Damage = 0.f;
		float Speed = 0.f;
		float SizeOnSweep = 0.f;
		float ForwarningTime = 0.f;

		bool StabbyTime = false;
		bool DoingAttack = false;
		bool PreparingSweep = false;

		CU::Vec3f LeftPoint;
		CU::Vec3f MiddlePoint;
		CU::Vec3f RightPoint;
		CU::Vec3f StartPoint;
		CU::Vec3f EndPoint;

		EntityID AttackStayID;
	};
	struct StabAndStuck
	{
		float StabDamage = 0.f;
		float HeadDamage = 0.f;
		float AttackStayTime = 0.f;
		float StuckTime = 0.f;
		float ExtraDamage = 0.f;
		float StabForwarningTime = 0.f;
		float HeadForwarningTime = 0.f;
		float SizePerStabAttack = 0.f;
		float SizePerHeadAttack = 0.f;
		float DelayBetweenStabs = 0.f;
		float DelayBeforeHeadAttack = 0.f;
		float RandomStabPlacementRange = 0.f;
		int StabCount = 0;
		int StabsLeft = 0;

		int StabIdCount = 0;

		bool DoHeadAttack = false;
		bool PreparingHead = false;
		bool IsStuck = false;
		bool UnderStabDelay = false;
		bool ReadyToStab = true;
		bool CalmBeforeTheStorm = false;
		bool AnimationPlayed = false;

		CU::Vec3f HeadHitPoint;
	};
	struct RoarHelp
	{
		float DelayBetweenSpawns = 0.f;
		float SpawnPushToMiddle = 0.f;

		int SpawnPoolSize = 0;

		unsigned int LurkersLeft = 0;
		unsigned int HuntersLeft = 0;

		bool ReadyToSpawn = true;
		bool DelayOnSpawn = false;

		CU::Vec2i SpawnCountRangeLurker;
		CU::Vec2i SpawnCountRangeHunter;

		CU::Vec3f MiddlePoint;

		std::vector<CU::Vec3f> SpawnPool;
	};
}
