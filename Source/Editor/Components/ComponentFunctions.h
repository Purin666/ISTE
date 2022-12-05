#pragma once
#include <string>
#include "imgui/imgui.h"
#include "Commands/CommandManager.h"
#include "Commands/VariableCommand.h"
namespace EF //Editor Functions
{
	template <typename T>
	void SetList(int aCurrentValue, T& aVariableToChange, std::string& aListType, std::string someCorrelatingStrings[], T somePossibleEnums[]) // This was a headache to develop, even though it's impractical this was the best I could do.
	{
		ImGui::Text(aListType.c_str());
		if (ImGui::TreeNode(someCorrelatingStrings[aCurrentValue].c_str()))
		{
			for (int i = 0; i < (*(&someCorrelatingStrings + 1)) - someCorrelatingStrings; i++)
			{
				if (ImGui::Selectable(someCorrelatingStrings[i].c_str()) && i != aCurrentValue)
				{
					CommandManager::DoCommand(new VariableCommand<T>(&aVariableToChange, somePossibleEnums[aCurrentValue], somePossibleEnums[i]));
					aVariableToChange = somePossibleEnums[i];
					ImGui::TreePop();
					return;
				}
			}
			ImGui::TreePop();
		}
	};
}
