#pragma once
#include "AbstractTool.h"

#include "ISTE/UI/UIDefines.h"

namespace ISTE
{
	struct Context;
	
	using UITable = std::unordered_map<std::string, UI>;

	enum class SelectedEdit
	{
		Images,
		Texts,
		Buttons,
		GameplayImages,
		Tooltips,

		Count
	};

	class UITool : public AbstractTool
	{
	public:
		void Init(BaseEditor*) override;
		void Draw() override;

		void Control(UITable&);
		void Control(ImageTable&);
		void Control(TextTable&);
		void Control(ButtonTable&);
		void Control(GameplayTable&);
		void Control(TooltipTable&);

		void Edit(Image&);
		void Edit(Text&);
		void Edit(Button&);
		void Edit(GameplayImage&);
		void Edit(Tooltip&);

		void RefreshSelected();

	private:
		void RemoveDoubleBackslah(std::string& text);

		BaseEditor* myBaseEditor;

		Context* myCtx = nullptr;

		SelectedEdit mySelectedEdit = SelectedEdit::Count;

		std::string mySelectedUI = "N/A";
		std::string mySelectedImage = "N/A";
		std::string mySelectedText = "N/A";
		std::string mySelectedButton = "N/A";
		std::string mySelectedGameplayImage = "N/A";
		std::string mySelectedTooltip = "N/A";

		size_t myTTimageIndex = 0;
		size_t myTTtextIndex = 0;
	};
}