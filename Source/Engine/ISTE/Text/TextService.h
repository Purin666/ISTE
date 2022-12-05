#pragma once
#include <string>
#include <wrl/client.h>  

#include "ISTE/CU/HashMap.hpp"
#include "ISTE/Math/Vec2.h"
#include "ISTE/Text/TextRenderCommand.h"
#include "ISTE/Graphics/ComponentAndSystem/Sprite2DCommand.h"
#include "ISTE/Graphics/RenderDefines.h"



/*
	for the time being, its all here.

	todo: Load fonts 
*/
#define MaxFontCount 64


using Microsoft::WRL::ComPtr;

struct FT_LibraryRec_;
struct ID3D11ShaderResourceView;

namespace ISTE
{
	struct Context;
	class TextService
	{
	public:
		struct FontLoadResult {
			FontID								myFontID			= FontID(-1);
			ComPtr<ID3D11ShaderResourceView>	myFontTexture		= nullptr;
			bool								mySuccessFlag		= false;
		};


		struct CharData {
			CU::Vec2f myTopLeftUV;
			CU::Vec2f myBottomRightUV;

			short myWidth;
			short myHeight;
			short myAdvanceX; //Distance to next character.
			short myAdvanceY;
			short myBearingX;
			short myBearingY;
			char myChar;
		};

		struct Font {
			CharData myCharData[1024];
			unsigned int myAtlasWidth;
			unsigned int myAtlasHeight;
			float myLineSpacing;
			unsigned int myWordSpacing;
			unsigned int myFontHeightWidth;
			std::vector<int> myAtlas;
			std::string myName;
			TextureID myFontAtlasID;
			FontID myFontId;
		};


	public:
		TextService();
		~TextService();

		void Init();

		FontID CheckIfExists(std::string aPath, unsigned int aFontSize, unsigned int aBorderSize = 0);
		FontLoadResult LoadFont(std::string aPath, unsigned int aFontSize, unsigned int aBorderSize = 0);
		Font* GetFontData(FontID it) { return myLoadedFonts.GetByIterator(it); }
		CU::HashMap<std::string, Font*, MaxFontCount, FontID>& GetFontList() { return myLoadedFonts; }

		Sprite2DRenderCommand processNextCharacter(const ISTE::TextService::Font& fontData, char aCharacter, float aSize, float& x, float& y, float& maxY, float aSS = 0, float aWS = 0, float aLS = 0);

		void PsBindFontTexture(FontID it, unsigned int aSlot);

	private:
		Context* myCtx;
		FT_LibraryRec_* myLibrary;
		CU::HashMap<std::string, Font*, MaxFontCount, FontID> myLoadedFonts;

	};

};

