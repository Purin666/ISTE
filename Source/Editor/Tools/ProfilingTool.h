#pragma once
#include "AbstractTool.h"
namespace ISTE
{
	class ProfilingTool : public AbstractTool
	{
	public:
		void Init(BaseEditor*) override;
		void Draw() override;

	private:
		void FPS();
		void DrawCalls();
		void Allocations();
		void Entities();

		float myElepsedTime = 0;
		int myFpsCount = 0;
		int myLastFps = 0 ;
	};
}