
#include "StateStack.h"
#include "StateType.h"

#include "ISTE/States/State_Play.h"
#include "ISTE/States/State_HUD.h"

#include "ISTE/States/State_MenuMain.h"
#include "ISTE/States/State_MenuLevelSelect.h"
#include "ISTE/States/State_MenuSettings.h"
#include "ISTE/States/State_MenuCredits.h"
#include "ISTE/States/State_MenuStats.h"
#include "ISTE/States/State_MenuPause.h"

#include "ISTE/States/State_SplashScreen.h"
#include "ISTE/States/State_FadeIn.h"
#include "ISTE/States/State_FadeOut.h"
#include "ISTE/States/State_Win.h"
#include "ISTE/States/State_Lose.h"


namespace ISTE
{
	void RegisterStates(StateStack& aStack)
	{
		// init stack
		for (size_t i = 0; i < globalStateCount; ++i)
			aStack.myStates[i].myType = static_cast<StateType>(i);

		// register

		RegisterState<State_Play>(aStack, StateType::Play);
		RegisterState<State_HUD>(aStack, StateType::HUD);
		
		RegisterState<State_MenuMain>(aStack, StateType::MenuMain);
		RegisterState<State_MenuLevelSelect>(aStack, StateType::MenuLevelSelect);
		RegisterState<State_MenuSettings>(aStack, StateType::MenuSettings);
		RegisterState<State_MenuCredits>(aStack, StateType::MenuCredits);
		RegisterState<State_MenuStats>(aStack, StateType::MenuStats);
		RegisterState<State_MenuPause>(aStack, StateType::MenuPause);
		
		RegisterState<State_SplashScreen>(aStack, StateType::SplashScreen);
		RegisterState<State_FadeIn>(aStack, StateType::FadeIn);
		RegisterState<State_FadeOut>(aStack, StateType::FadeOut);
		RegisterState<State_Win>(aStack, StateType::Win);
		RegisterState<State_Lose>(aStack, StateType::Lose);
	}
}