#pragma once
#include "AbstractTool.h"

namespace ISTE
{
	struct Context;

	class StateTool : public AbstractTool
	{
	public:
		void Init(BaseEditor*) override;
		void Draw() override;

	private:
		Context* myCtx = nullptr;

	};
}