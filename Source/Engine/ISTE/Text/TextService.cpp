#include "TextService.h"
#include <d3d11.h>
#include <ft2build.h>
#include <freetype/freetype.h>


#include "ISTE/Context.h"
#include "ISTE/Graphics/DX11.h"
#include "ISTE/Logger/Logger.h"
#include "ISTE/Graphics/ComponentAndSystem/Sprite2DCommand.h"
#include "ISTE/WindowsWindow.h"

#include FT_FREETYPE_H
#include FT_GLYPH_H
#include FT_BITMAP_H
#include FT_STROKER_H
#include FT_OUTLINE_H

#define X_OFFSET 8
#define X_START 2
#define Y_START 2
#define Y_OFFSET 8

#pragma region GlyphLoader
struct TextToRender
{
	CU::Vec2f mySize;
	CU::Vec2f myUvStart;
	CU::Vec2f myUvEnd;
	CU::Vec2f myPosition;
	bool isWhitespace;
};

struct GlyphLoader
{
	struct CountData
	{
		int xNCount = 0;
		int xPCount = 0;
		int yNCount = 0;
		int yPCount = 0;
	};

	struct OutlineOffset
	{
		int xDelta = 0;
		int yDelta = 0;

	};

	void LoadGlyph(int index, int& atlasX, int& atlasY, int& maxY, float atlasWidth, float atlasHeight, ISTE::TextService::Font* aFontData, struct FT_FaceRec_* aFace, int aBorderWidth = 0);
	void LoadOutline(const int index, const int atlasX, const int atlasY, const float atlasWidth, ISTE::TextService::Font* aFontData, struct FT_FaceRec_* aFace, int aBorderWidth);
	void CalculateOutlineOffsets(const int index, struct FT_FaceRec_* aFace, int aBorderWidth);
	void CalculateGlyphOffsets(const int index, struct FT_GlyphSlotRec_* glyph);

	void CountOffsets(const int& x, const int& y, const int& width, const int& height, CountData& countData);
	void CountDeltas(const int& width, const int& height, int& deltaX, int& deltaY, CountData& countData);

	OutlineOffset myTextOutlineOffset;
	struct FT_LibraryRec_* myLibrary;
};

void GlyphLoader::CountOffsets(const int& x, const int& y, const int& width, const int& height, CountData& countData)
{
	if (x < 1)
	{
		countData.xNCount++;
	}

	if (x > width - 1)
	{
		countData.xPCount++;
	}

	if (y < 1)
	{
		countData.yNCount++;
	}

	if (y > height - 1)
	{
		countData.yPCount++;
	}
}

void GlyphLoader::CountDeltas(const int& width, const int& height, int& deltaX, int& deltaY, CountData& countData)
{
	if (countData.xNCount == height)
	{
		countData.xNCount = 0;
		deltaX--;
	}

	if (countData.xPCount == height)
	{
		countData.xPCount = 0;
		deltaX++;
	}

	if (countData.yNCount == width)
	{
		countData.yNCount = 0;
		deltaY--;
	}

	if (countData.yPCount == width)
	{
		countData.yPCount = 0;
		deltaY++;
	}
}

void GlyphLoader::CalculateOutlineOffsets(const int index, FT_FaceRec_* aFace, int aBorderOffset)
{ 
	myTextOutlineOffset.xDelta = 0;
	myTextOutlineOffset.yDelta = 0;

	FT_Error err;
	FT_Stroker stroker;
	FT_Glyph glyph;

	err = FT_Load_Char(aFace, index, FT_LOAD_NO_BITMAP);
	if (err != 0)
	{

		ISTE::Logger::DebugOutputError(L"Failed to load glyph!");
	}
	err = FT_Get_Glyph(aFace->glyph, &glyph);

	glyph->format = FT_GLYPH_FORMAT_OUTLINE;

	err = FT_Stroker_New(myLibrary, &stroker);
	if (err != 0)
	{
		ISTE::Logger::DebugOutputError(L"Failed to get glyph!");
	}


	FT_Stroker_Set(stroker, aBorderOffset * 64, FT_STROKER_LINECAP_ROUND, FT_STROKER_LINEJOIN_ROUND, 0);
	err = FT_Glyph_StrokeBorder(&glyph, stroker, 0, 1);
	if (err != 0)
	{
		ISTE::Logger::DebugOutputError(L"Failed to stroke glyph border");
	}


	err = FT_Glyph_To_Bitmap(&glyph, FT_RENDER_MODE_NORMAL, nullptr, true);
	if (err != 0)
	{
		ISTE::Logger::DebugOutputError(L"Failed to add glyph to bitmap");
	}

	FT_BitmapGlyph bitmapGlyph = reinterpret_cast<FT_BitmapGlyph>(glyph);

	unsigned int width = bitmapGlyph->bitmap.width;
	unsigned int height = bitmapGlyph->bitmap.rows;

	int xDelta = 0;
	int yDelta = 0;

	CountData countData;
	for (unsigned int x = 0; x < width; x++)
	{
		for (unsigned int y = 0; y < height; y++)
		{
			unsigned char& data = bitmapGlyph->bitmap.buffer[y * width + x];

			if (data == 0)
			{
				CountOffsets(x, y, width, height, countData);
			}
			CountDeltas(width, height, xDelta, yDelta, countData);
		}
	}

	myTextOutlineOffset.xDelta = xDelta;
	myTextOutlineOffset.yDelta = yDelta;

	FT_Stroker_Done(stroker);
	FT_Done_Glyph(glyph);
}

void GlyphLoader::CalculateGlyphOffsets(const int /*index*/, FT_GlyphSlotRec_* glyph)
{
	int xDelta = 0;
	int yDelta = 0;

	unsigned int width = glyph->bitmap.width;
	unsigned int height = glyph->bitmap.rows;

	CountData countData;
	for (unsigned int x = 0; x < width; x++)
	{
		for (unsigned int y = 0; y < height; y++)
		{
			unsigned char& data = glyph->bitmap.buffer[y * width + x];

			if (data == 0)
			{
				CountOffsets(x, y, width, height, countData);
			}
			CountDeltas(width, height, xDelta, yDelta, countData);
		}
	}

	myTextOutlineOffset.xDelta = xDelta;
	myTextOutlineOffset.yDelta = yDelta;
}

int Color32Reverse2(int x)
{
	return ((x & 0xFF000000) >> 8) | ((x & 0x00FF0000) >> 8) | ((x & 0x0000FF00) << 8) | ((x & 0x000000FF) << 24);
}

int Color32Reverse(int x)
{
	int returner = x;
	returner |= ((x & 0x000000FF) << 24);
	returner |= ((x & 0x000000FF) << 16);
	returner |= ((x & 0x000000FF) << 8);

	return returner;
}

void GlyphLoader::LoadGlyph(int index, int& atlasX, int& atlasY, int& maxY, float atlasWidth, float atlasHeight, ISTE::TextService::Font* aFontData, FT_FaceRec_* aFace, int aBorderOffset)
{  
	FT_Error error = FT_Load_Char(aFace, index, FT_LOAD_RENDER);
	if (error != 0)
	{
		ISTE::Logger::DebugOutputError(L"Failed to load char while glyph loading");
		return;
	}

	FT_GlyphSlot slot = aFace->glyph;
	FT_Bitmap bitmap = slot->bitmap;

	int height = bitmap.rows;
	int width = bitmap.width;

	ISTE::TextService::CharData glyphData;
	glyphData.myChar = static_cast<char>(index);
	glyphData.myHeight = static_cast<short>(height + (aBorderOffset * 2));
	glyphData.myWidth = static_cast<short>(width + (aBorderOffset * 2));

	glyphData.myTopLeftUV = { (float(atlasX) / atlasWidth), (float(atlasY) / atlasHeight) };
	glyphData.myBottomRightUV = { (float(atlasX + glyphData.myWidth) / atlasWidth), (float(atlasY + glyphData.myHeight) / atlasHeight) };

	glyphData.myAdvanceX = static_cast<short>(slot->advance.x) >> 6;
	glyphData.myAdvanceY = static_cast<short>(slot->advance.y) >> 6;

	glyphData.myBearingX = (short)slot->bitmap_left;
	glyphData.myBearingY = (short)slot->bitmap_top;
	if (glyphData.myTopLeftUV.x > 1 || glyphData.myTopLeftUV.y > 1 || glyphData.myBottomRightUV.x > 1 || glyphData.myBottomRightUV.y > 1)
	{
		ISTE::Logger::DebugOutputError(L"Tried to set a glyph UV to above 1");
		return;
	}

	CalculateGlyphOffsets(index, slot);
	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height; y++)
		{
			if (x < 0 || y < 0)
			{
				continue;
			}
			int& saved = aFontData->myAtlas[((atlasY)+aBorderOffset + myTextOutlineOffset.yDelta + y) *
				int(atlasWidth) + ((atlasX + aBorderOffset + myTextOutlineOffset.xDelta) + x)];
			saved |= bitmap.buffer[y * bitmap.width + x];

			saved = Color32Reverse(saved);

			if (y + (atlasY + Y_OFFSET) > maxY)
			{
				maxY = y + (atlasY + Y_OFFSET);
			}
		}
	}

	atlasX = atlasX + width + X_OFFSET;
	aFontData->myCharData[index] = glyphData;
}

void GlyphLoader::LoadOutline(const int index, const int atlasX, const int atlasY, const float atlasWidth, ISTE::TextService::Font* aFontData, FT_FaceRec_* aFace, int aBorderOffset)
{ 
	FT_Error err;
	FT_Stroker stroker;
	FT_Glyph glyph;


	err = FT_Load_Char(aFace, index, FT_LOAD_NO_BITMAP);
	if (err != 0)
	{
		ISTE::Logger::DebugOutputError(L"Failed to load glyph!");
	}
	 

	err = FT_Get_Glyph(aFace->glyph, &glyph);
	if (err != 0)
	{
		ISTE::Logger::DebugOutputError(L"Failed to get glyph!");
	}

	glyph->format = FT_GLYPH_FORMAT_OUTLINE;

	err = FT_Stroker_New(myLibrary, &stroker);
	if (err != 0)
	{
		ISTE::Logger::DebugOutputError(L"Failed to get stroker!");
	} 

	FT_Stroker_Set(stroker, aBorderOffset * 64, FT_STROKER_LINECAP_ROUND, FT_STROKER_LINEJOIN_ROUND, 0);
	err = FT_Glyph_StrokeBorder(&glyph, stroker, 0, 1);
	if (err != 0)
	{
		ISTE::Logger::DebugOutputError(L"Failed to stroke!");
	}

	err = FT_Glyph_To_Bitmap(&glyph, FT_RENDER_MODE_NORMAL, nullptr, true);
	if (err != 0)
	{
		ISTE::Logger::DebugOutputError(L"Failed to add glyph to bitmap");
	} 

	CalculateOutlineOffsets(index, aFace, aBorderOffset); 

	FT_BitmapGlyph bitmapGlyph = reinterpret_cast<FT_BitmapGlyph>(glyph);
	unsigned int width	= bitmapGlyph->bitmap.width;
	unsigned int height	= bitmapGlyph->bitmap.rows;

	for (unsigned int x = 0; x < width; x++)
	{
		for (unsigned int y = 0; y < height; y++)
		{

			int& data = aFontData->myAtlas[((atlasY + myTextOutlineOffset.yDelta) + y) * int(atlasWidth) + ((atlasX + myTextOutlineOffset.xDelta) + x)];
			data = 0;
			data |= bitmapGlyph->bitmap.buffer[y * width + x];
			data = Color32Reverse2(data);

		}
	} 
	FT_Stroker_Done(stroker);
	FT_Done_Glyph(glyph);
}

ISTE::Sprite2DRenderCommand ISTE::TextService::processNextCharacter(const ISTE::TextService::Font& fontData, char aCharacter, float aSize, float& x, float& y, float& maxY, float aSS, float aWS, float aLS)
{
	CU::Vec2Ui res = Context::Get()->myWindow->GetResolution();

	float screenSizeX = static_cast<float>(res.x);
	float screenSizeY = static_cast<float>(res.y); 

	float screenScaleRatioX = screenSizeX / 1280.f;
	float screenScaleRatioY = screenSizeY / 720.f;

	int charAsNumber = (unsigned char)aCharacter;
	const ISTE::TextService::CharData& charData = fontData.myCharData[charAsNumber];

	if (maxY < charData.myHeight)
	{
		maxY = static_cast<float>(charData.myHeight);
	}

	Sprite2DRenderCommand result = {};

	 result.myPosition.Set((x + charData.myBearingX) * aSize * screenScaleRatioX, (y + charData.myBearingY) * aSize * screenScaleRatioY);

	result.myScale.Set(static_cast<float>(charData.myWidth), static_cast<float>(charData.myHeight));

	result.myUVStart = charData.myTopLeftUV;
	result.myUVEnd = charData.myBottomRightUV;

	if (aCharacter == '\n')
	{
		x = 0;
		y -= (fontData.myLineSpacing * 3 + aLS);
	}
	else
	{
		if (aCharacter == ' ')
		{
			x += fontData.myWordSpacing + aSS;
		}
		x += (charData.myAdvanceX + aWS);
	}

	return result;
}

void ISTE::TextService::PsBindFontTexture(FontID it, unsigned int aSlot)
{
	ID3D11DeviceContext* aContext = myCtx->myDX11->GetContext();

	//aContext->PSSetShaderResources(aSlot, 1, myLoadedFonts.GetByIterator((int)it)->myAtlasView.GetAddressOf());
}

#pragma endregion 

ISTE::TextService::TextService()
{
	myLibrary = nullptr;
}

ISTE::TextService::~TextService()
{
	FT_Done_FreeType(myLibrary);
	myLoadedFonts.ClearFromHeap();
}

void ISTE::TextService::Init()
{
	FT_Init_FreeType(&myLibrary);  
	myCtx = ISTE::Context::Get();
}
FontID ISTE::TextService::CheckIfExists(std::string aPath, unsigned int aFontSize, unsigned int aBorderSize)
{
	std::string key = aPath;
	key = key + "|" + std::to_string(aFontSize) + "|" + std::to_string(aBorderSize);

	FontLoadResult output;
	if (myLoadedFonts.Exists(key)) 
		return myLoadedFonts.Get(key)->myFontId; 

	return FontID(-1);
}

ISTE::TextService::FontLoadResult ISTE::TextService::LoadFont(std::string aPath, unsigned int aFontSize, unsigned int aBorderSize)
{

	short fontWidth = (short)aFontSize;
	const int atlasSize = 1024;
	float atlasWidth = static_cast<float>(atlasSize);
	float atlasHeight = static_cast<float>(atlasSize);

	int atlasX = X_START;
	int atlasY = Y_START;
	int currentMaxY = 0;

	//Loads the font 
	Font* createdFont = new Font();
	createdFont->myAtlas.resize(atlasWidth * atlasHeight,0);
	createdFont->myFontHeightWidth = fontWidth;
	createdFont->myName = aPath; 

	FT_Face face;

	//loads face
	FT_Error error = FT_New_Face(myLibrary, aPath.c_str(), 0, &face);
	if (error != 0)
	{
#ifdef _DEBUG
		std::cout << "Failed to create font" << std::endl;
		Logger::DebugOutputError(L"Failed to create font");
#endif 
		delete createdFont;
		return {};
	}

	//sets font size
	FT_F26Dot6 ftSize = (FT_F26Dot6)(createdFont->myFontHeightWidth * (1 << 6));
	error = FT_Set_Char_Size(face, ftSize, 0, 100, 100);
	if (error != 0)
	{
		Logger::DebugOutputError(L"FT_Set_Char_Size error");
		delete createdFont;
		return {};
	}


	int currentMax = 256;
	int currentI = 32;

	//Loads the glyphs and sets 
	GlyphLoader gLoader;
	gLoader.myLibrary = myLibrary;
	for (int i = currentI; i < currentMax; i++)
	{
		error = FT_Load_Char(face, i, FT_LOAD_RENDER);
		if (error != 0)
		{ 
			continue;
		}

		FT_GlyphSlot slot = face->glyph;
		if (atlasX + slot->bitmap.width + (aBorderSize * 2) > atlasWidth)
		{
			atlasX = 2;
			atlasY = currentMaxY;
		}

		if (aBorderSize > 0)
		{
			gLoader.LoadOutline(i, atlasX, atlasY, atlasWidth, createdFont, face, aBorderSize);
		} 
		gLoader.LoadGlyph(i, atlasX, atlasY, currentMaxY, atlasWidth, atlasHeight, createdFont, face, aBorderSize);
	}  
	//sets the word spacing
	FT_GlyphSlot space = face->glyph;
	createdFont->myWordSpacing = static_cast<short>(space->metrics.width / 256.f);

	//Creates FonteAtlas
	{
	ID3D11DeviceContext* context = Context::Get()->myDX11->GetContext();
	ID3D11Device* device = Context::Get()->myDX11->GetDevice();

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = createdFont->myAtlas.data();
	data.SysMemPitch = atlasSize * 4;

	D3D11_TEXTURE2D_DESC info;
	info.Width = atlasSize;
	info.Height = atlasSize;
	info.MipLevels = 0;
	info.ArraySize = 1;
	info.SampleDesc.Count = 1;
	info.SampleDesc.Quality = 0;
	info.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
	info.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	info.Usage = D3D11_USAGE_DEFAULT;
	info.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	info.CPUAccessFlags = 0;
	
	FontLoadResult output;
	ID3D11Texture2D* texture;
	HRESULT hr = device->CreateTexture2D(&info, nullptr, &texture);
	if (FAILED(hr))
	{
#ifdef _DEBUG
		std::cout << "Failed to create font" << std::endl;
		Logger::DebugOutputError(L"Failed to create font");
#endif  

		delete createdFont;
		return { FontID(-1) };
	}

	context->UpdateSubresource(texture, 0, NULL, createdFont->myAtlas.data(), atlasSize * 4, 0); 

	device->CreateShaderResourceView(texture, nullptr, output.myFontTexture.GetAddressOf());
	context->GenerateMips(output.myFontTexture.Get());
	texture->Release();

	createdFont->myAtlasHeight = atlasSize;
	createdFont->myAtlasWidth = atlasSize;
	createdFont->myLineSpacing = static_cast<float>((face->ascender - face->descender) >> 6);
	FT_Done_Face(face); 

	std::string key = aPath;
	key = key + "|" + std::to_string(aFontSize) + "|" + std::to_string(aBorderSize);
	FontID id = myLoadedFonts.Insert(key, createdFont);
	createdFont->myFontId = id;

	output.myFontID = id; 
	output.mySuccessFlag = true;
	return output;
	}
}