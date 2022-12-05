#pragma once

namespace ISTE
{
	typedef unsigned int VisualizationStateFlag;

	//should probably just bitshift for easy use
	enum class VisualizationFlagValues
	{
		eNavMesh = 1 << 0,
		eTriggers = 1 << 1,
		eSkeleton = 1 << 2,
		eFull = (1 << 4) - 1//this should always be the last
	};

	class VisualizationSettings
	{
	public:
		void StateCheck();
		void DrawVisuals();


	private:
		void DrawNavMesh();
		void DrawSkeleton();


	private:
#ifdef _DEBUG
		VisualizationStateFlag myCoverageFlag = (VisualizationStateFlag)VisualizationFlagValues::eFull;
#else
		VisualizationStateFlag myCoverageFlag = 0;
#endif // _DEBUG



	};
}