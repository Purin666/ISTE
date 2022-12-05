
#include "ISTE/UI/UIWorker.h"
#include "ISTE/UI/UIDefines.h"
#include "ISTE/UI/UIHandler.h"

#include "ISTE/Context.h"
#include "ISTE/ECSB/SystemManager.h"
#include "ISTE/Graphics/ComponentAndSystem/SpriteDrawerSystem.h"
#include "ISTE/Text/TextSystem.h"

#include "ISTE/CU/InputHandler.h"
#include "ISTE/CU/RayFromScreen.h"
#include "ISTE/WindowsWindow.h"

#include "ISTE/CU/Geometry/Intersection.h"

#include "ISTE/CU/MemTrack.hpp"

constexpr float ASPECT_RATIO = 16.f / 9.f;

namespace
{
	void ProcessButtons(ISTE::UI& aUI)
	{
		ISTE::Context* ctx = ISTE::Context::Get();

		// mouse info
		ISTE::ScreenHelper helper;
		const POINT point = helper.GetMouseRelativePosition();
		const CU::Vec2Ui res = ctx->myWindow->GetResolution();
		const float x = (float)point.x / (float)res.x * 2.f - 1.f;
		const float y = (float)point.y / (float)res.y * 2.f - 1.f;
		const CU::Vec2f mousePos(x, -y);
		const bool mouseDown = ctx->myInputHandler->IsKeyDown(VK_LBUTTON);
		const bool mouseHeld = ctx->myInputHandler->IsKeyHeldDown(VK_LBUTTON);
		const bool mouseUp = ctx->myInputHandler->IsKeyUp(VK_LBUTTON);
		//

		for (auto& [name, button] : aUI.myButtons)
		{
			// min, max
			const CU::Vec2f& pos = button.myImage.myCommand.myPosition;
			const CU::Vec2f& size = button.myImage.myCommand.myScale;
			const CU::Vec2f& scale = button.myHitboxScale;
			const CU::Vec2f min = { pos.x - size.x * scale.x / ASPECT_RATIO, pos.y - size.y * scale.y };
			const CU::Vec2f max = { pos.x + size.x * scale.x / ASPECT_RATIO, pos.y + size.y * scale.y };
			//
			if (mouseUp)
			{
				if (CU::IsInside(mousePos, min, max))
				{
					ctx->myUIHandler->AddCallbackToExecute(button.myCallback);
					return;
				}
			}
			// quick solve enter/exit check
			if (IsInside(mousePos, min, max))
			{
				if (mouseDown)
					button.myImage.myCommand.myColor = button.myImage.myClickColor;

				if (!button.myMouseIsInside)
				{
					button.myMouseIsInside = true;
					if (mouseHeld)
						button.myImage.myCommand.myColor = button.myImage.myClickColor;
					button.myImage.myCommand.myColor += button.myImage.myHightlightColor;

					// activate tooltip now
				}
				if (mouseUp)
					button.myImage.myCommand.myColor = button.myImage.myDefaultColor + button.myImage.myHightlightColor;
			}
			else
			{
				if (button.myMouseIsInside)
				{
					button.myMouseIsInside = false;
					button.myImage.myCommand.myColor = button.myImage.myDefaultColor;

					// deactivate tooltip now
				}
			}
		}
		for (auto& [name, image] : aUI.myGameplayImages)
		{	
			// min, max
			auto& com = image.myImage.myCommand;

			const CU::Vec2f& pos = com.myPosition;
			const CU::Vec2f& size = com.myScale;
			const CU::Vec2f& scale = image.myHitboxScale;
			const CU::Vec2f min = { pos.x - size.x * scale.x / ASPECT_RATIO, pos.y - size.y * scale.y };
			const CU::Vec2f max = { pos.x + size.x * scale.x / ASPECT_RATIO, pos.y + size.y * scale.y };

			if (IsInside(mousePos, min, max))
			{
				switch (image.myType)
				{
				case ISTE::GameplayUIType::AbilityL:
					ISTE::AddRenderCommands(aUI.myTooltips["Tooltip_Primary"]);
					break;
				case ISTE::GameplayUIType::AbilityR:
					ISTE::AddRenderCommands(aUI.myTooltips["Tooltip_Secondary"]);
					break;
				case ISTE::GameplayUIType::Ability1:
					ISTE::AddRenderCommands(aUI.myTooltips["Tooltip_Armor"]);
					break;
				case ISTE::GameplayUIType::Ability2:
					ISTE::AddRenderCommands(aUI.myTooltips["Tooltip_AoEDoT"]);
					break;
				case ISTE::GameplayUIType::Ability3:
					ISTE::AddRenderCommands(aUI.myTooltips["Tooltip_Teleport"]);
					break;
				case ISTE::GameplayUIType::Ability4:
					ISTE::AddRenderCommands(aUI.myTooltips["Tooltip_Ultimate"]);
					break;
				}
			}
		}
	}
}

void ISTE::ProcessUI(UI& aUI)
{
	// Set color of images
	for (auto& [name, image] : aUI.myImages)
		image.myCommand.myColor = image.myDefaultColor;

	for (auto& [name, image] : aUI.myGameplayImages)
		image.myImage.myCommand.myColor = image.myImage.myDefaultColor;

	//if (!aUI.myButtons.empty())
	ProcessButtons(aUI);
}

void ISTE::AddRenderCommands(const UI& aUI)
{
	Context* ctx = Context::Get();
	SpriteDrawerSystem* spriteDrawer = ctx->mySystemManager->GetSystem<SpriteDrawerSystem>();
	TextSystem* textDrawer = ctx->mySystemManager->GetSystem<TextSystem>();

	for (const auto& [name, image] : aUI.myGameplayImages)
	{
		if (image.myImage.myCommand.myTextureId[ALBEDO_MAP] >= 0)
		{
			auto com = image.myImage.myCommand;
			com.myScale.x /= ASPECT_RATIO;
			spriteDrawer->Add2DSpriteRenderCommand(com);
			//spriteDrawer->Add2DSpriteRenderCommand(image.myImage.myCommand);
		}
	}

	for (const auto& [name, image] : aUI.myImages)
	{
		if (image.myCommand.myTextureId[ALBEDO_MAP] >= 0)
		{
			auto com = image.myCommand;
			com.myScale.x /= ASPECT_RATIO;
			spriteDrawer->Add2DSpriteRenderCommand(com);
			//spriteDrawer->Add2DSpriteRenderCommand(image.myCommand);
		}
	}

	for (const auto& [name, button] : aUI.myButtons)
	{
		if (button.myImage.myCommand.myTextureId[ALBEDO_MAP] >= 0)
		{
			auto com = button.myImage.myCommand;
			com.myScale.x /= ASPECT_RATIO;
			spriteDrawer->Add2DSpriteRenderCommand(com);
			//spriteDrawer->Add2DSpriteRenderCommand(button.myImage.myCommand);
		}
	}

	for (const auto& [name, text] : aUI.myTexts)
	{
		if (text.myCommand.myFontID >= 0)
			textDrawer->AddTextCommand(text.myCommand);
	}

}

void ISTE::AddRenderCommands(const Tooltip& tooltip)
{
	Context* ctx = Context::Get();
	SpriteDrawerSystem* spriteDrawer = ctx->mySystemManager->GetSystem<SpriteDrawerSystem>();
	TextSystem* textDrawer = ctx->mySystemManager->GetSystem<TextSystem>();

	for (auto& image : tooltip.myImages)
	{
		auto com = image.myCommand;
		com.myScale.x /= ASPECT_RATIO;
		spriteDrawer->Add2DSpriteRenderCommand(com);
	}
	for (const auto& text : tooltip.myTexts)
		textDrawer->AddTextCommand(text.myCommand);
}
