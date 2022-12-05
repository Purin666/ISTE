#pragma once


namespace ISTE
{
	typedef unsigned int IconStateFlag;

	//should probably just bitshift for easy use
	enum class FlagValues
	{
		eLights			= 1 << 0,
		eEmittier		= 1 << 1,
		eAudioSource	= 1 << 2,
		eFull			= (1 << 3) - 1//this should always be the last
	};

	class IconSettings
	{
	public:
		void StateCheck();
		void DrawIcons();


	private:
		IconStateFlag myCoverageFlag = (IconStateFlag)FlagValues::eFull;
	};
}