#pragma once
#include "ISTE/Graphics/RenderStateEnums.h"
#include "ISTE/Math/Vec.h"
#include "ISTE/Graphics/RenderDefines.h"
namespace ISTE
{
	struct TextRenderCommand {
		std::string myText;
		CU::Vec2f	myPivot;
		CU::Vec4f	myColor			= { 1.f, 1.f, 1.f, 1.f };
		CU::Vec2f	myPosition		= { 0.f, 0.f };
		float		myRotation		= 1.f; 
		float		myTextScale		= 1; 
		float		myLetterSpacing	= 0;
		float		myWordSpacing	= 0;
		float		myLineSpacing	= 0;
		FontID		myFontID		= -1;

		SamplerState mySamplerState = SamplerState::eBiliniear;
		AdressMode myAdressMode = AdressMode::eClamp;
		BlendState myBlendState = BlendState::eAlphaBlend;
	};
};	