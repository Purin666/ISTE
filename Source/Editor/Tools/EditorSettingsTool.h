#pragma once
#include "AbstractTool.h"

#include <vector>

namespace ISTE
{

	class EditorSettingsTool : public AbstractTool
	{
	public:
		void Init(BaseEditor* anEditor) override;
		void Draw() override;

	private:

		std::vector<AbstractTool*> myTools;
	};
}