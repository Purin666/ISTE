#pragma once
#include "AbstractTool.h"

#include <unordered_map>
#include <vector>
#include "ISTE/Math/Vec3.h"

namespace ISTE
{
	struct Context;

	struct PlayerSnapPos
	{
		CU::Vec3f myPosition;
		std::string myName;

	};

	class PlayerTool : public AbstractTool
	{
	public:
		void Init(BaseEditor*) override;
		void Draw() override;

	private:
		Context* myCtx = nullptr;
		void DrawPlayerCheatTool();
		void DrawAnimBlendTool();


		//player snap tings
		std::unordered_map<std::string, std::vector<PlayerSnapPos>> myPlayerSnapPos;

		std::string myCurrentName = "";

		void SaveToJson();
		void SaveAnimBlendDataToJson();
		void ReadJson();

	};
}