#pragma once

namespace ISTE
{
	enum class StateType
	{
		Play,
		HUD,

		MenuMain,
		MenuLevelSelect,
		MenuSettings,
		MenuCredits,
		MenuStats,
		MenuPause,

		SplashScreen,
		FadeIn,
		FadeOut,

		Win,
		Lose,

		Count
	};

	inline constexpr size_t globalStateCount = static_cast<size_t>(ISTE::StateType::Count);
}			