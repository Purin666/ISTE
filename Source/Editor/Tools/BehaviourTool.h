#pragma once
#include "AbstractTool.h"
#include <vector>
#include <list>
namespace ISTE
{
	struct Context;
	class BehaviourTool : public AbstractTool
	{
	public:
		~BehaviourTool();
		void Draw() override;
		void Init(BaseEditor*) override;
	private:
		void IdleEdit();
		void LurkerEdit();
		void HunterEdit();
		void BossEdit();
		void PlayerEdit();
		void EliteEdit();
		void SaveBehaviours();
		const char* myBehaviours[6]{"Hunter", "Lurker", "Player", "Boss", "Elites", "Idle"};
		const char* myBossStages[4]{ "100% HP", "75% HP", "50% HP", "25% HP" };
		int mySelectedBehaviour = 0;
		int mySelectedBossStage = 0;
		float myDragSensitivity = 1.f;
		//std::list<std::vector<int>*> myTemporaryStorage;
	};


}