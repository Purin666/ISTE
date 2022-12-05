#pragma once
#include "AbstractTool.h"


namespace ISTE
{
	struct Context;

	class Particle2DTool : public AbstractTool
	{
	public:
		void Init(BaseEditor*) override;
		void Draw() override;

	private:
		Context* myCtx = nullptr;

		std::string mySelectedData = "N/A";
		int myEmitterId = -1;
	};
}