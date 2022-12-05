#pragma once
#include <wrl/client.h> 

#include <string>

#include <ISTE/CU/HashMap.hpp>
#include <ISTE/Math/Vec2.h>

#include <ISTE/Graphics/RenderDefines.h>


using Microsoft::WRL::ComPtr;
struct ID3D11ShaderResourceView;
namespace ISTE
{
	struct Context;
	class TextService;
	class Font;
	class TextureManager
	{
	//private://structs
	public:
		struct Texture {

			ComPtr<ID3D11ShaderResourceView> mySRV;
			CU::Vec2Ui myTexSize;
			std::wstring myFilePath;
			TextureID myIndex;
		};


	public:
		struct TextureLoadResult {
			operator TextureID() { return myValue; }
			operator bool() { return mySuccesFlag; }

			bool mySuccesFlag;
			TextureID myValue;
		};

	public:
		TextureManager();
		~TextureManager();

		bool Init();

		void VsBindTexture(TextureID TextureID, unsigned int aSlot);
		void PsBindTexture(TextureID TextureID, unsigned int aSlot);

		TextureLoadResult LoadTexture(std::wstring aTexturePath, bool aSrgbFlag = false, bool aMipMapFlag = false);  
		FontID LoadFont(std::string  aTexturePath, size_t aFontSize = 18, size_t aFontBorderSize = 0);

		TextService* GetTextService() { return myTextServicer; }
		Texture& GetTexture(TextureID it) { return myTextures.GetByIterator(it); }
		CU::HashMap<std::wstring, Texture, MAX_TEXTURE_COUNT, TextureID>& GetTextureList() { return myTextures; }

	private:
		TextService* myTextServicer;
		Context* myCtx;
		CU::HashMap<std::wstring, Texture, MAX_TEXTURE_COUNT, TextureID> myTextures;

		ID3D11ShaderResourceView* myNullRSV = nullptr;

		TextureLoadResult	LoadNewTexture(const wchar_t* aTexturePath, bool aSrgbFlag = true, bool aMipMapFlag = false);
		FontID				LoadNewFont(const char* aTexturePath, size_t aFontSize, size_t aFontBorderSize);
		
		HRESULT ReadWICTexture(const wchar_t* aFilePath, ComPtr<ID3D11ShaderResourceView>& aSRV, bool aSrgbFlag, bool aMipMapFlag);
		HRESULT ReadDDSTexture(const wchar_t* aFilePath, ComPtr<ID3D11ShaderResourceView>& aSRV, bool aSrgbFlag, bool aMipMapFlag);
	};
}