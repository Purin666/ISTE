#pragma once
#include <string>

#include "ISTE/Math/Vec.h"
namespace ISTE
{
	struct TextComponent {
		~TextComponent()
		{
			myText.clear();
		}
		std::string myText;
		CU::Vec2f	myPivot;
		CU::Vec4f	myColor = {1.f,1.f,1.f,1.f};
		float		myLetterSpacing = 0;
		float		myWordSpacing = 0;
		float		myLineSpacing = 0;
		float		myScale = 1.f;
		FontID		myFontID = -1;
	};
}