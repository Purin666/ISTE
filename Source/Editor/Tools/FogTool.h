#pragma once
#include "AbstractTool.h"
namespace ISTE
{
	struct Context;
	class FogTool : public AbstractTool
	{
	public:
		void Draw() override;
		void Init(BaseEditor*) override;
	private:

	};


}