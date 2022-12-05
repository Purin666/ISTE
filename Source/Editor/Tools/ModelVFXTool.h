#pragma once
#include "AbstractTool.h"

#include "ISTE/ECSB/ECSDefines.hpp"

namespace ISTE
{
	struct Context;

	class ModelVFXTool : public AbstractTool
	{
	public:
		void Init(BaseEditor*) override;
		void Draw() override;

	private:
		Context* myCtx = nullptr;

		std::string mySelectedData = "N/A";
		EntityID myEmitterId = INVALID_ENTITY;
	};
}