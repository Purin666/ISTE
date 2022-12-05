#include "StatsFondler.h"

#include "ISTE/Context.h"
#include "ISTE/CU/Database.h"
#include "ISTE/CU/ReadWrite.h"
#include "ISTE/Events/EventHandler.h"
#include "ISTE/Time/TimeHandler.h"


StatsFondler::~StatsFondler()
{
	SaveData();
	UnRegisterEvents();
}

void StatsFondler::UpdateDatabase()
{
	CU::Database<true>* db = ISTE::Context::Get()->myGenericDatabase;
	db->SetValue("GameStats", myGameStats);
	std::cout << myGameStats << std::endl;
}

void StatsFondler::Init()
{
	ISTE::EventHandler* eh = ISTE::Context::Get()->myEventHandler;
	assert(eh, "Tell a Programmer");
		
	LoadData();
	RegisterEvents();
	UpdateDatabase();
}

void StatsFondler::LoadData()
{
	if (!ReadWrite::LoadFile(myFilePath)) { SaveData(); return; }
	ReadWrite::Read<GameStats>(myGameStats);
	ReadWrite::Flush();
	std::cout << "StatsFondler loaded data from " << myFilePath << std::endl;
}

void StatsFondler::SaveData()
{
	ReadWrite::Que<GameStats>(myGameStats);
	ReadWrite::SaveToFile(myFilePath);
	std::cout << "StatsFondler saved data to " << myFilePath << std::endl;
}

void StatsFondler::RegisterEvents()
{
	ISTE::EventHandler* eh = ISTE::Context::Get()->myEventHandler;
	
	eh->RegisterCallback(ISTE::EventType::PlayerDied, "StatsFondler", [this](EntityID id) { myGameStats.myPlayerDeaths++; });
	eh->RegisterCallback(ISTE::EventType::TimDied,	  "StatsFondler", [this](EntityID id) { myGameStats.myTimDeaths++; });
	eh->RegisterCallback(ISTE::EventType::BorisDied,  "StatsFondler", [this](EntityID id) { myGameStats.myBorisDeaths++; });
	eh->RegisterCallback(ISTE::EventType::BobDied,    "StatsFondler", [this](EntityID id) { myGameStats.myBobDeaths++; });
	eh->RegisterCallback(ISTE::EventType::PotDied,    "StatsFondler", [this](EntityID id) { myGameStats.myPotDeaths++; });

	eh->RegisterCallback(ISTE::EventType::PlayerUsedAbility, "StatsFondler", [this](EntityID id) {
		switch ((int)id)
		{
		case 0: myGameStats.myMeleeUsed++; break;
		case 1: myGameStats.myRangedUsed++; break;
		case 2: myGameStats.myAbility1Used++; break;
		case 3: myGameStats.myAbility2Used++; break;
		case 4: myGameStats.myAbility3Used++; break;
		case 5: myGameStats.myAbility4Used++; break;
		}
	});

	eh->RegisterCallback(ISTE::EventType::LevelCompleted, "StatsFondler", [this](EntityID id) {
		const float time = ISTE::Context::Get()->myTimeHandler->GetTime("Played_Time_Current_Level"); // idk just temp the timer doesnt actually exist
		ISTE::Context::Get()->myTimeHandler->RemoveTimer("Played_Time_Current_Level");
		switch ((int)id)
		{
		case 1: if (time < myGameStats.myBestTimeMainGate)	   myGameStats.myBestTimeMainGate	  = time; break;
		case 2: if (time < myGameStats.myBestTimeOldTown)	   myGameStats.myBestTimeOldTown	  = time; break;
		case 3: if (time < myGameStats.myBestTimeCatacombs)	   myGameStats.myBestTimeCatacombs	  = time; break;
		case 4: if (time < myGameStats.myBestTimeTheDepths)	   myGameStats.myBestTimeTheDepths	  = time; break;
		case 6: if (time < myGameStats.myBestTimeAbyssalShelf) myGameStats.myBestTimeAbyssalShelf = time; break;
		}
		myGameStats.myTotalPlayTime += time;
		UpdateDatabase();
		std::cout << "Played_Time_Current_Level: " << time << std::endl;
	});


}

void StatsFondler::UnRegisterEvents()
{
	ISTE::EventHandler* eh = ISTE::Context::Get()->myEventHandler;

	eh->RemoveCallback(ISTE::EventType::PlayerDied, "StatsFondler");
	eh->RemoveCallback(ISTE::EventType::TimDied, "StatsFondler");
	eh->RemoveCallback(ISTE::EventType::BorisDied, "StatsFondler");
	eh->RemoveCallback(ISTE::EventType::BobDied, "StatsFondler");
	eh->RemoveCallback(ISTE::EventType::PotDied, "StatsFondler");
	eh->RemoveCallback(ISTE::EventType::PlayerUsedAbility, "StatsFondler");
	eh->RemoveCallback(ISTE::EventType::LevelCompleted, "StatsFondler");

	//eh->RemoveCallback(ISTE::EventType::PlayerTookDamage, "StatsFondler");
	//eh->RemoveCallback(ISTE::EventType::MinionTookDamage, "StatsFondler");
	//eh->RemoveCallback(ISTE::EventType::BossTookDamage, "StatsFondler");
	//eh->RemoveCallback(ISTE::EventType::PlayerSpentMana, "StatsFondler");
	//eh->RemoveCallback(ISTE::EventType::TimEliteDied, "StatsFondler");
	//eh->RemoveCallback(ISTE::EventType::BorisEliteDied, "StatsFondler");
	//eh->RemoveCallback(ISTE::EventType::PlayerMoveDistance, "StatsFondler");
	//eh->RemoveCallback(ISTE::EventType::PlayerTeleportedDistance, "StatsFondler");
	//eh->RemoveCallback(ISTE::EventType::PlayerAbsorbedDamage, "StatsFondler");
	//eh->RemoveCallback(ISTE::EventType::PlayerGainExperience, "StatsFondler");
	
}
