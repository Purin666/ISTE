#pragma once
#include "imgui/imgui.h"
#include "CommandManager.h"
#include "VariableCommand.h"
namespace CF //Command Functions
{


	template<typename T>
	void CheckEditing(T& anOriginalValue, T* aDestination, bool& anEditActive)
	{
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			anEditActive = false;
			CommandManager::DoCommand(new VariableCommand<T>(aDestination, anOriginalValue, *aDestination));
		}
		if (ImGui::IsItemActivated() && !anEditActive)
		{
			anEditActive = true;
			anOriginalValue = *aDestination;
		}
	};
	template<typename T>
	void CheckEditing(T& anOriginalValue, T* aDestination, T aResultValue, bool& anEditActive)
	{
		if (ImGui::IsItemDeactivated())
		{
			anEditActive = false;
			CommandManager::DoCommand(new VariableCommand<T>(aDestination, anOriginalValue, aResultValue));
		}
		if (ImGui::IsItemActivated() && !anEditActive)
		{
			anEditActive = true;
			anOriginalValue = *aDestination;
		}
	};
}