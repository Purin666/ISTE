#pragma once

#include "ISTE/ECSB/ECSDefines.hpp"

#include "ISTE/Graphics/ComponentAndSystem/Sprite2DCommand.h"
#include "ISTE/Text/TextRenderCommand.h"

#include <functional>

namespace ISTE
{
	enum class GameplayUIType
	{
		AbilityL, 
		AbilityR,
		Ability1,
		Ability2,
		Ability3,
		Ability4,
		// don't touch above
		PlayerHealth,
		PlayerMana,
		PlayerExperience,
		BossHealth,
		
		Count
	};
	struct Image
	{
		Sprite2DRenderCommand myCommand;

		CU::Vec4f myDefaultColor = { 1, 1, 1, 1 };
		CU::Vec4f myHightlightColor = { 0.3f, 0.3f, 0.3f, 0 };
		CU::Vec4f myClickColor = { 1, 1, 1, 0 };

		std::string myPath = "../Assets/Sprites/UI/Ui_Button.dds";
	};
	struct Text
	{
		TextRenderCommand myCommand;

		std::string  myPath = "../Assets/Font/arial.ttf";
		size_t mySize = 18;
		size_t myBorder = 2;
	};
	struct Button
	{
		Image myImage;
		CU::Vec2f myHitboxScale	= { 1, 1 };

		std::string myCallbackName = "../Assets/Sprites/UI/Ui_Button.dds";
		std::function<void()> myCallback = nullptr;

		bool myMouseIsInside = false;
	};
	struct GameplayImage
	{
		Image myImage;
		CU::Vec2f myHitboxScale = { 1, 1 };
		GameplayUIType myType = GameplayUIType::Count;

		bool myMouseIsInside = false;
	};
	struct Tooltip
	{
		std::vector<Image> myImages;
		std::vector<Text> myTexts;
	};

	using ImageTable = std::unordered_map<std::string, Image>;
	using TextTable = std::unordered_map<std::string, Text>;
	using ButtonTable = std::unordered_map<std::string, Button>;
	using GameplayTable = std::unordered_map<std::string, GameplayImage>;
	using TooltipTable = std::unordered_map<std::string, Tooltip>;

	struct UI
	{
		ImageTable		myImages;
		TextTable		myTexts;
		ButtonTable		myButtons;
		GameplayTable	myGameplayImages;
		TooltipTable	myTooltips;
	};
}