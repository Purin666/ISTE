#include "UITool.h"

#include "imgui/imgui.h"
#include "imgui/misc/cpp/imgui_stdlib.h"

#include "ISTE/Helper/JsonIO.h"
#include "ISTE/Helper/JsonDefines.h" 

#include "ISTE/Context.h"

#include "ISTE/Graphics/Resources/TextureManager.h"
#include "ISTE/UI/UIHandler.h"
#include "ISTE/Scene/Scene.h"
#include "ISTE/Scene/SceneHandler.h"

#include "ISTE/UI/UIWorker.h"

#include "ISTE/States/State_MenuMain.h"
#include "ISTE/StateStack/StateManager.h"
#include "ISTE/StateStack/StateType.h"

#include "ISTE/CU/UtilityFunctions.hpp"

#include "BaseEditor.h"

void ISTE::UITool::Init(BaseEditor* aBaseEditor)
{
	myBaseEditor = aBaseEditor;

	myToolName = "UI Tool";
	myCtx = Context::Get();

	auto& datas = myCtx->myUIHandler->myUIDatas;

	//LoadJson("../Assets/MiscData/UIData.json", datas);

	if (!datas.empty())
	{
		mySelectedUI = datas.begin()->first;
		RefreshSelected();
	}
}

void ISTE::UITool::Draw()
{
	if (!myActive) return;

	auto& uiDatas = myCtx->myUIHandler->myUIDatas;
	
	ImGui::Begin("UI Tool", &myActive);

	if (ImGui::Button("Reload from JSON"))
	{
		LoadJson("../Assets/MiscData/UIData.json", uiDatas);
		myCtx->myUIHandler->LoadTexturesAndFonts();
		RefreshSelected();
	}

	Control(uiDatas);

	if (!uiDatas.empty())
	{
		if (ImGui::Button("Edit Images"))
			mySelectedEdit = SelectedEdit::Images;
		ImGui::SameLine();
		if (ImGui::Button("Edit Texts"))
			mySelectedEdit = SelectedEdit::Texts;
		ImGui::SameLine();
		if (ImGui::Button("Edit Buttons"))
			mySelectedEdit = SelectedEdit::Buttons;
		ImGui::SameLine();
		if (ImGui::Button("Edit GameplayImages"))
			mySelectedEdit = SelectedEdit::GameplayImages;
		if (ImGui::Button("Edit Tooltip"))
			mySelectedEdit = SelectedEdit::Tooltips;

		ImGui::Separator();

		auto& ui = uiDatas[mySelectedUI];

		switch (mySelectedEdit)
		{
		case ISTE::SelectedEdit::Images:
			Control(ui.myImages);
			ImGui::Separator();
			if (!ui.myImages.empty()) 
				Edit(ui.myImages[mySelectedImage]);
			break;
		case ISTE::SelectedEdit::Texts:
			Control(ui.myTexts);
			ImGui::Separator();
			if (!ui.myTexts.empty())
				Edit(ui.myTexts[mySelectedText]);
			break;
		case ISTE::SelectedEdit::Buttons:
			Control(ui.myButtons);
			ImGui::Separator();
			if (!ui.myButtons.empty())
				Edit(ui.myButtons[mySelectedButton]);
			break;
		case ISTE::SelectedEdit::GameplayImages:
			Control(ui.myGameplayImages);
			ImGui::Separator();
			if (!ui.myGameplayImages.empty())
				Edit(ui.myGameplayImages[mySelectedGameplayImage]);
			break;
		case ISTE::SelectedEdit::Tooltips:
			Control(ui.myTooltips);
			ImGui::Separator();
			if (!ui.myTooltips.empty())
				Edit(ui.myTooltips[mySelectedTooltip]);
			break;
		case ISTE::SelectedEdit::Count:
			break;
		}
		ProcessUI(ui);
		AddRenderCommands(ui);
		if (!ui.myTooltips.empty())
			AddRenderCommands(ui.myTooltips[mySelectedTooltip]);
	}
	ImGui::End();
}

void ISTE::UITool::Control(UITable& uiTable)
{	
	static std::string newUIName;

	if (ImGui::Button("Save All"))
		SaveJson("../Assets/MiscData/UIData.json", uiTable);

	ImGui::SameLine();
	if (ImGui::Button("New"))
	{
		newUIName = "Default_" + std::to_string(uiTable.size());
		uiTable[newUIName] = UI();
		mySelectedUI = newUIName;
		RefreshSelected();
	}

	ImGui::SameLine();
	if (ImGui::Button("Remove"))
	{
		uiTable.erase(mySelectedUI);
		if (uiTable.empty())
			mySelectedUI = "N/A";
		else
			mySelectedUI = uiTable.begin()->first;

		RefreshSelected();
	}

	ImGui::SameLine();
	if (!uiTable.empty())
	{
		if (ImGui::Button("Duplicate UI"))
		{
			newUIName = mySelectedUI;
			const size_t selectedSize = mySelectedUI.size();
			size_t suffix = 0;
			for (auto& ui : uiTable)
			{
				if (ui.first.substr(0, selectedSize) == mySelectedUI)
					suffix++;
			}
			newUIName = newUIName + std::to_string(suffix);
			uiTable[newUIName] = uiTable[mySelectedUI];
			mySelectedUI = newUIName;
		}
	}

	if (ImGui::BeginCombo("Selected UI", mySelectedUI.data()))
	{
		for (auto& data : uiTable)
		{
			const bool selected = (mySelectedUI == data.first);

			if (ImGui::Selectable(data.first.data(), selected))
			{
				mySelectedUI = data.first;
				RefreshSelected();
			}

			if (selected)
				ImGui::SetItemDefaultFocus();
		}

		ImGui::EndCombo();
	}

	ImGui::InputText("Rename UI", &newUIName, ImGuiInputTextFlags_EnterReturnsTrue);
	if (ImGui::IsItemDeactivatedAfterEdit())
	{
		if (uiTable.count(newUIName) == 0)
		{
			uiTable[newUIName] = uiTable[mySelectedUI];
			uiTable.erase(mySelectedUI);
			mySelectedUI = newUIName;
			newUIName = "";
			RefreshSelected();
		}
	}
}

void ISTE::UITool::Control(ImageTable& images)
{
	static std::string newImageName;
	
	ImGui::PushID(&images);

	if (ImGui::Button("New"))
	{
		newImageName = "Image_" + std::to_string(images.size());
		images[newImageName] = Image();
		images[newImageName].myCommand.myScale *= 0.1f;
		images[newImageName].myCommand.myTextureId[ALBEDO_MAP] = myCtx->myTextureManager->LoadTexture(std::wstring(images[newImageName].myPath.begin(), images[newImageName].myPath.end())).myValue;
		mySelectedImage = newImageName;
	}

	ImGui::SameLine();
	if (ImGui::Button("Remove"))
	{
		images.erase(mySelectedImage);
		if (images.empty())
			mySelectedImage = "N/A";
		else
			mySelectedImage = images.begin()->first;
	}

	ImGui::SameLine();
	if (!images.empty())
	{
		if (ImGui::Button("Duplicate"))
		{
			newImageName = mySelectedImage;
			const size_t selectedSize = mySelectedImage.size();
			size_t suffix = 0;
			for (auto& image : images)
			{
				if (image.first.substr(0, selectedSize) == mySelectedImage)
					suffix++;
			}
			newImageName = newImageName + std::to_string(suffix);
			images[newImageName] = images[mySelectedImage];
			mySelectedImage = newImageName;
		}
	}

	if (ImGui::BeginCombo("Selected", mySelectedImage.data()))
	{
		for (auto& image : images)
		{
			const bool selected = (mySelectedImage == image.first);

			if (ImGui::Selectable(image.first.data(), selected))
				mySelectedImage = image.first;

			if (selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}

	ImGui::InputText("Rename", &newImageName, ImGuiInputTextFlags_EnterReturnsTrue);
	if (ImGui::IsItemDeactivatedAfterEdit())
	{
		if (images.count(newImageName) == 0)
		{
			images[newImageName] = images[mySelectedImage];
			images.erase(mySelectedImage);
			mySelectedImage = newImageName;
			newImageName = "";
		}
	}
	ImGui::PopID();
}

void ISTE::UITool::Control(TextTable& texts)
{
	static std::string newTextName;

	ImGui::PushID(&texts);

	if (ImGui::Button("New"))
	{
		newTextName = "Text_" + std::to_string(texts.size());
		auto& text = texts[newTextName];
		text = Text();
		text.myCommand.myFontID = Context::Get()->myTextureManager->LoadFont(text.myPath, text.mySize, text.myBorder);
		mySelectedText = newTextName;
	}

	ImGui::SameLine();
	if (ImGui::Button("Remove"))
	{
		texts.erase(mySelectedText);
		if (texts.empty())
			mySelectedText = "N/A";
		else
			mySelectedText = texts.begin()->first;
	}

	ImGui::SameLine();
	if (!texts.empty())
	{
		if (ImGui::Button("Duplicate"))
		{
			newTextName = mySelectedText;
			const size_t selectedSize = mySelectedText.size();
			size_t suffix = 0;
			for (auto& text : texts)
			{
				if (text.first.substr(0, selectedSize) == mySelectedText)
					suffix++;
			}
			newTextName = newTextName + std::to_string(suffix);
			texts[newTextName] = texts[mySelectedText];
			mySelectedText = newTextName;
		}
	}

	if (ImGui::BeginCombo("Selected", mySelectedText.data()))
	{
		for (auto& text : texts)
		{
			const bool selected = (mySelectedText == text.first);

			if (ImGui::Selectable(text.first.data(), selected))
			{
				mySelectedText = text.first;
			}

			if (selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}

	ImGui::InputText("Rename", &newTextName, ImGuiInputTextFlags_EnterReturnsTrue);
	if (ImGui::IsItemDeactivatedAfterEdit())
	{
		if (texts.count(newTextName) == 0)
		{
			texts[newTextName] = texts[mySelectedText];
			texts.erase(mySelectedText);
			mySelectedText = newTextName;
			newTextName = "";
		}
	}

	ImGui::PopID();
}

void ISTE::UITool::Control(ButtonTable& buttons)
{
	static std::string newButtonName;
	
	ImGui::PushID(&buttons);

	if (ImGui::Button("New"))
	{
		newButtonName = "Button_" + std::to_string(buttons.size());
		buttons[newButtonName] = Button();
		buttons[newButtonName].myImage.myCommand.myScale *= 0.1f;
		buttons[newButtonName].myImage.myCommand.myTextureId[ALBEDO_MAP] = myCtx->myTextureManager->LoadTexture(std::wstring(buttons[newButtonName].myImage.myPath.begin(), buttons[newButtonName].myImage.myPath.end())).myValue;
		mySelectedButton = newButtonName;
	}

	ImGui::SameLine();
	if (ImGui::Button("Remove"))
	{
		buttons.erase(mySelectedButton);
		if (buttons.empty())
			mySelectedButton = "N/A";
		else
			mySelectedButton = buttons.begin()->first;
	}

	ImGui::SameLine();
	if (!buttons.empty())
	{
		if (ImGui::Button("Duplicate"))
		{
			newButtonName = mySelectedButton;
			const size_t selectedSize = mySelectedButton.size();
			size_t suffix = 0;
			for (auto& button : buttons)
			{
				if (button.first.substr(0, selectedSize) == mySelectedButton)
					suffix++;
			}
			newButtonName = newButtonName + std::to_string(suffix);
			buttons[newButtonName] = buttons[mySelectedButton];
			mySelectedButton = newButtonName;
		}
	}

	if (ImGui::BeginCombo("Selected", mySelectedButton.data()))
	{
		for (auto& button : buttons)
		{
			const bool selected = (mySelectedButton == button.first);

			if (ImGui::Selectable(button.first.data(), selected))
				mySelectedButton = button.first;

			if (selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}

	ImGui::InputText("Rename", &newButtonName, ImGuiInputTextFlags_EnterReturnsTrue);
	if (ImGui::IsItemDeactivatedAfterEdit())
	{
		if (buttons.count(newButtonName) == 0)
		{
			buttons[newButtonName] = buttons[mySelectedButton];
			buttons.erase(mySelectedButton);
			mySelectedButton = newButtonName;
			newButtonName = "";
		}
	}
	ImGui::PopID();
}

void ISTE::UITool::Control(GameplayTable& images)
{
	static std::string newGameplayImageName;

	ImGui::PushID(&images);

	if (ImGui::Button("New"))
	{
		newGameplayImageName = "GameplayImage_" + std::to_string(images.size());
		images[newGameplayImageName] = GameplayImage();
		images[newGameplayImageName].myImage.myCommand.myScale *= 0.1f;
		images[newGameplayImageName].myImage.myCommand.myTextureId[ALBEDO_MAP] = myCtx->myTextureManager->LoadTexture(std::wstring(images[newGameplayImageName].myImage.myPath.begin(), images[newGameplayImageName].myImage.myPath.end())).myValue;
		mySelectedGameplayImage = newGameplayImageName;
	}

	ImGui::SameLine();
	if (ImGui::Button("Remove"))
	{
		images.erase(mySelectedGameplayImage);
		if (images.empty())
			mySelectedGameplayImage = "N/A";
		else
			mySelectedGameplayImage = images.begin()->first;
	}

	ImGui::SameLine();
	if (!images.empty())
	{
		if (ImGui::Button("Duplicate"))
		{
			newGameplayImageName = mySelectedGameplayImage;
			const size_t selectedSize = mySelectedGameplayImage.size();
			size_t suffix = 0;
			for (auto& button : images)
			{
				if (button.first.substr(0, selectedSize) == mySelectedGameplayImage)
					suffix++;
			}
			newGameplayImageName = newGameplayImageName + std::to_string(suffix);
			images[newGameplayImageName] = images[mySelectedGameplayImage];
			mySelectedGameplayImage = newGameplayImageName;
		}
	}

	if (ImGui::BeginCombo("Selected", mySelectedGameplayImage.data()))
	{
		for (auto& button : images)
		{
			const bool selected = (mySelectedGameplayImage == button.first);

			if (ImGui::Selectable(button.first.data(), selected))
				mySelectedGameplayImage = button.first;

			if (selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}

	ImGui::InputText("Rename", &newGameplayImageName, ImGuiInputTextFlags_EnterReturnsTrue);
	if (ImGui::IsItemDeactivatedAfterEdit())
	{
		if (images.count(newGameplayImageName) == 0)
		{
			images[newGameplayImageName] = images[mySelectedGameplayImage];
			images.erase(mySelectedGameplayImage);
			mySelectedGameplayImage = newGameplayImageName;
			newGameplayImageName = "";
		}
	}
	ImGui::PopID();
}

void ISTE::UITool::Control(TooltipTable& tootips)
{
	static std::string newTooltipName;

	ImGui::PushID(&tootips);

	if (ImGui::Button("New"))
	{
		newTooltipName = "Tooltip_" + std::to_string(tootips.size());
		auto& tt = tootips[newTooltipName];
		tt = Tooltip();

		tt.myImages.push_back(Image());
		auto& image = tt.myImages.back();
		image.myCommand.myScale *= 0.1f;
		image.myCommand.myTextureId[ALBEDO_MAP] = myCtx->myTextureManager->LoadTexture(std::wstring(image.myPath.begin(), image.myPath.end())).myValue;
		
		tt.myTexts.push_back(Text());
		auto& text = tt.myTexts.back();
		text.myCommand.myFontID = Context::Get()->myTextureManager->LoadFont(text.myPath, text.mySize, text.myBorder);

		mySelectedTooltip = newTooltipName;
	}

	ImGui::SameLine();
	if (ImGui::Button("Remove"))
	{
		tootips.erase(mySelectedTooltip);
		if (tootips.empty())
			mySelectedTooltip = "N/A";
		else
			mySelectedTooltip = tootips.begin()->first;
	}

	ImGui::SameLine();
	if (!tootips.empty())
	{
		if (ImGui::Button("Duplicate"))
		{
			newTooltipName = mySelectedTooltip;
			const size_t selectedSize = mySelectedTooltip.size();
			size_t suffix = 0;
			for (auto& button : tootips)
			{
				if (button.first.substr(0, selectedSize) == mySelectedTooltip)
					suffix++;
			}
			newTooltipName = newTooltipName + std::to_string(suffix);
			tootips[newTooltipName] = tootips[mySelectedTooltip];
			mySelectedTooltip = newTooltipName;
		}
	}

	if (ImGui::BeginCombo("Selected", mySelectedTooltip.data()))
	{
		for (auto& button : tootips)
		{
			const bool selected = (mySelectedTooltip == button.first);

			if (ImGui::Selectable(button.first.data(), selected))
				mySelectedTooltip = button.first;

			if (selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}

	ImGui::InputText("Rename", &newTooltipName, ImGuiInputTextFlags_EnterReturnsTrue);
	if (ImGui::IsItemDeactivatedAfterEdit())
	{
		if (tootips.count(newTooltipName) == 0)
		{
			tootips[newTooltipName] = tootips[mySelectedTooltip];
			tootips.erase(mySelectedTooltip);
			mySelectedTooltip = newTooltipName;
			newTooltipName = "";
		}
	}
	ImGui::PopID();
}

void ISTE::UITool::Edit(Image& image)
{
	auto& com = image.myCommand;

	// Texture Path
	ImGui::PushID(&image);
	ImGui::InputText("Texture Path", image.myPath.data(), image.myPath.size());
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(".dds", 0))
		{
			image.myPath = *(std::string*)payload->Data;
			com.myTextureId[ALBEDO_MAP] = Context::Get()->myTextureManager->LoadTexture(std::wstring(image.myPath.begin(), image.myPath.end()));
		}
	}
	if (ImGui::GetIO().MouseDoubleClicked[0] && ImGui::IsItemClicked())
	{
		myBaseEditor->GetAssetsManager().LookUp(image.myPath);
	}
	// Transform
	ImGui::DragFloat2("myPosition", (float*)&com.myPosition, 0.001f);
	ImGui::DragFloat2("myPivot", (float*)&com.myPivot, 0.001f);
	ImGui::DragFloat2("myScale", (float*)&com.myScale, 0.001f, 0.f);
	ImGui::DragFloat("myRotation", (float*)&com.myRotation, 0.001f);
	// UV
	ImGui::DragFloat2("myUVStart", (float*)&com.myUVStart, 0.001f, 0.f, 1.f);
	ImGui::DragFloat2("myUVEnd", (float*)&com.myUVEnd, 0.001f, 0.f, 1.f);
	ImGui::DragFloat2("myUVOffset", (float*)&com.myUVOffset, 0.001f);
	ImGui::DragFloat2("myUVScale", (float*)&com.myUVScale, 0.001f, 0.f);
	// Colors
	ImGui::DragFloat4("myDefaultColor", (float*)&image.myDefaultColor, 0.01f, 0.f);
	ImGui::DragFloat4("myHightlightColor", (float*)&image.myHightlightColor, 0.01f, 0.f);
	ImGui::DragFloat4("myClickColor", (float*)&image.myClickColor, 0.01f, 0.f);
	ImGui::PopID();
}

void ISTE::UITool::Edit(Text& text)
{
	auto& com = text.myCommand;
	static bool updateFontID = false;

	// Font path, size, border
	ImGui::PushID(&text);
	ImGui::Text("Text: ");
	ImGui::SameLine();
	ImGui::Text(com.myText.data());
	static std::string newTextText;
	ImGui::InputText("Set New Text", &newTextText, ImGuiInputTextFlags_EnterReturnsTrue);
	if (ImGui::IsItemDeactivatedAfterEdit())
	{
		com.myText = newTextText;
		newTextText = "";
	}
	ImGui::InputText("myPath", text.myPath.data(), text.myPath.size());
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(".ttf", 0))
		{
			text.myPath = *(std::string*)payload->Data;
			updateFontID = true;
		}
	}
	if (ImGui::GetIO().MouseDoubleClicked[0] && ImGui::IsItemClicked())
	{
		myBaseEditor->GetAssetsManager().LookUp(text.myPath);
	}
	updateFontID += ImGui::DragInt("mySize", (int*)&text.mySize, 0.05f, 0, 72);
	updateFontID += ImGui::DragInt("myBorder", (int*)&text.myBorder, 0.05f, 0, 3);
	//static size_t* vars[2];
	//vars[0] = &text.mySize;
	//vars[1] = &text.myBorder;
	//updateFontID += ImGui::DragInt2("Size & Border", (int*)vars[0], 0.01f, 0, 72);

	if (updateFontID)
		com.myFontID = Context::Get()->myTextureManager->LoadFont(text.myPath, text.mySize, text.myBorder);

	ImGui::DragFloat2("myPosition", (float*)&com.myPosition, 0.001f);
	ImGui::DragFloat("myTextScale", (float*)&com.myTextScale, 0.001f, 0.f);
	ImGui::DragFloat("myRotation", (float*)&com.myRotation, 0.001f);
	ImGui::DragFloat2("myPivot", (float*)&com.myPivot, 0.001f);
	ImGui::DragFloat4("myDefaultColor", (float*)&com.myColor, 0.01f, 0.f);

	ImGui::PopID();
}

void ISTE::UITool::Edit(Button& button)
{
	Edit(button.myImage);

	ImGui::DragFloat2("myHitboxScale", (float*)&button.myHitboxScale, 0.001f);

	if (ImGui::BeginCombo("myCallbackName", button.myCallbackName.data()))
	{
		auto& callbacks = myCtx->myUIHandler->myCallbacks;
		for (auto& cb : callbacks)
		{
			const bool selected = (button.myCallbackName == cb.first);

			if (ImGui::Selectable(cb.first.data(), selected))
			{
				button.myCallbackName = cb.first;
				button.myCallback = callbacks[button.myCallbackName];
				myCtx->myUIHandler->LoadCallbacks();
			}

			if (selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}
}

void ISTE::UITool::Edit(GameplayImage& image)
{
	Edit(image.myImage);

	ImGui::DragFloat2("myHitboxScale", (float*)&image.myHitboxScale, 0.001f);

	auto& types = myCtx->myUIHandler->myGameplayTypes;

	if (ImGui::BeginCombo("myType", types[(GameplayUIType)image.myType].data()))
	{
		for (const auto& [type, name] : types)
		{
			const bool selected = (image.myType == (GameplayUIType)type);

			if (ImGui::Selectable(name.data(), selected))
				image.myType = type;

			if (selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}
}

void ISTE::UITool::Edit(Tooltip& tooltip)
{
	if (ImGui::Button("Add Image"))
	{
		tooltip.myImages.push_back(Image());
		auto& image = tooltip.myImages.back();
		image.myCommand.myScale *= 0.1f;
		image.myCommand.myTextureId[ALBEDO_MAP] = myCtx->myTextureManager->LoadTexture(std::wstring(image.myPath.begin(), image.myPath.end())).myValue;
		myTTimageIndex = tooltip.myImages.size() - 1;
	}
	if (ImGui::Button("Add Text"))
	{
		tooltip.myTexts.push_back(Text());
		auto& text = tooltip.myTexts.back();
		text.myCommand.myFontID = myCtx->myTextureManager->LoadFont(text.myPath, text.mySize, text.myBorder);
		myTTtextIndex = tooltip.myTexts.size() - 1;
	}
	ImGui::Text("Images");
	for (size_t i = 0; i < tooltip.myImages.size(); i++)
	{
		ImGui::SameLine();
		if (ImGui::Button(std::string("Image " + std::to_string(i)).c_str()))
			myTTimageIndex = i;
	}
	ImGui::Text("Texts");
	for (size_t i = 0; i < tooltip.myTexts.size(); i++)
	{
		ImGui::SameLine();
		if (ImGui::Button(std::string("Text " + std::to_string(i)).c_str()))
			myTTtextIndex = i;
	}

	if (!tooltip.myImages.empty())
		Edit(tooltip.myImages[myTTimageIndex]);
	if (!tooltip.myTexts.empty())
		Edit(tooltip.myTexts[myTTtextIndex]);

}

void ISTE::UITool::RefreshSelected()
{
	auto& datas = myCtx->myUIHandler->myUIDatas;
	if (!datas.empty())
	{
		auto& ui = datas[mySelectedUI];

		mySelectedImage			= "N/A";
		mySelectedText			= "N/A";
		mySelectedButton		= "N/A";
		mySelectedGameplayImage = "N/A";
		mySelectedTooltip		= "N/A";

		const auto& images = ui.myImages;
		if (!images.empty())
			mySelectedImage = images.begin()->first;

		const auto& texts = ui.myTexts;
		if (!texts.empty())
			mySelectedText = texts.begin()->first;

		const auto& buttons = ui.myButtons;
		if (!buttons.empty())
			mySelectedButton = buttons.begin()->first;

		const auto& gameplayImages = ui.myGameplayImages;
		if (!gameplayImages.empty())
			mySelectedGameplayImage = gameplayImages.begin()->first;

		const auto& tootips = ui.myTooltips;
		if (!tootips.empty())
			mySelectedTooltip = tootips.begin()->first;
	}
	else
		mySelectedUI = "N/A";
}

