#pragma once

namespace ISTE
{
	struct UI;
	struct Tooltip;

	void ProcessUI(UI& aUI);
	void AddRenderCommands(const UI& aUI);
	void AddRenderCommands(const Tooltip& tooltip);
}