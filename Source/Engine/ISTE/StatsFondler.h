#pragma once

#include <string>

#include <iostream>

class StatsFondler
{
public:
	StatsFondler() = default;
	~StatsFondler();

	void UpdateDatabase();
	void Init();

private:
	void LoadData();
	void SaveData();

	void RegisterEvents();
	void UnRegisterEvents();

private:
	struct GameStats
	{
		size_t myPlayerDeaths = 0;
		size_t myTimDeaths = 0;
		size_t myTimEliteDeaths = 0;
		size_t myBorisDeaths = 0;
		size_t myBorisEliteDeaths = 0;
		size_t myBobDeaths = 0;
		size_t myPotDeaths = 0;

		size_t myMeleeUsed = 0;
		size_t myRangedUsed = 0;
		size_t myAbility1Used = 0;
		size_t myAbility2Used = 0;
		size_t myAbility3Used = 0;
		size_t myAbility4Used = 0;

		size_t myPlayerDamageTaken = 0;
		size_t myPlayerDamageGiven = 0;
		size_t myPlayerManaSpent = 0;
		size_t myTotalExperienceGained = 0;

		float myTotalMoveDistance = 0.f;
		float myTotalTeleportedDistance = 0.f;
		float myLongestTeleportedDistance = 0.f;

		float myBestTimeMainGate = FLT_MAX;
		float myBestTimeOldTown = FLT_MAX;
		float myBestTimeCatacombs = FLT_MAX;
		float myBestTimeTheDepths = FLT_MAX;
		float myBestTimeAbyssalShelf = FLT_MAX;
		float myTotalPlayTime = 0.f;

		friend std::ostream& operator<<(std::ostream& aOut, const GameStats& stats)
		{
			return aOut <<
				"\n<------------ Printing GameStats ------------>\n" <<
				"myPlayerDeaths: " << "\t" << stats.myPlayerDeaths << "\n" <<
				"myTimDeaths: " << "\t" << stats.myTimDeaths << "\n" <<
				"myTimEliteDeaths: " << "\t" << stats.myTimEliteDeaths << "\n" <<
				"myBorisDeaths: " << "\t" << stats.myBorisDeaths << "\n" <<
				"myBorisEliteDeaths: " << "\t" << stats.myBorisEliteDeaths << "\n" <<
				"myBobDeaths: " << "\t" << stats.myBobDeaths << "\n" <<
				"myPotDeaths: " << "\t" << stats.myPotDeaths << "\n\n" <<

				"myMeleeUsed: " << "\t" << stats.myMeleeUsed << "\n" <<
				"myRangedUsed: " << "\t" << stats.myRangedUsed << "\n" <<
				"myAbility1Used: " << "\t" << stats.myAbility1Used << "\n" <<
				"myAbility2Used: " << "\t" << stats.myAbility2Used << "\n" <<
				"myAbility3Used: " << "\t" << stats.myAbility3Used << "\n" <<
				"myAbility4Used: " << "\t" << stats.myAbility4Used << "\n\n" <<

				"myPlayerDamageTaken: " << "\t" << stats.myPlayerDamageTaken << "\n" <<
				"myPlayerDamageGiven: " << "\t" << stats.myPlayerDamageGiven << "\n" <<
				"myPlayerManaSpent: " << "\t" << stats.myPlayerManaSpent << "\n" <<
				"myTotalExperienceGained: " << "\t" << stats.myTotalExperienceGained << "\n\n" <<

				"myBestTimeMainGate: " << "\t\t" << stats.myBestTimeMainGate << "\n" <<
				"myBestTimeOldTown: " << "\t\t" << stats.myBestTimeOldTown << "\n" <<
				"myBestTimeCatacombs: " << "\t\t" << stats.myBestTimeCatacombs << "\n" <<
				"myBestTimeTheDepths: " << "\t" << stats.myBestTimeTheDepths << "\n" <<
				"myBestTimeAbyssalShelf: " << "\t" << stats.myBestTimeAbyssalShelf << "\n" <<
				"myTotalPlayTime: " << "\t\t" << stats.myTotalPlayTime <<
				"\n<-------------------------------------------->\n";
		}
	};


	GameStats myGameStats;

	std::string myFilePath = "../Assets/MiscData/Game.stats";
};