#include "TextureManager.h"
#include <d3d11.h>
         
 
#include <DirectXTex/WICTextureLoader/WICTextureLoader11.h>
#include <DirectXTex/DDSTextureLoader/DDSTextureLoader11.h>
         
#include <ISTE/Context.h>
#include <ISTE/Graphics/DX11.h>
#include <ISTE/Logger/Logger.h>
#include <ISTE/Text/TextService.h>


#include <fstream>
#include "ISTE/Helper/StringCast.h"



ISTE::TextureManager::TextureManager()
{
    myCtx = nullptr;
    myTextServicer = nullptr;
}

ISTE::TextureManager::~TextureManager()
{
    if (myTextServicer)
        delete myTextServicer;

}

bool ISTE::TextureManager::Init()
{
    myCtx = Context::Get();
    myTextServicer = new TextService();
    myTextServicer->Init();
    return true;
}

void ISTE::TextureManager::VsBindTexture(TextureID it, unsigned int aSlot)
{
    ID3D11DeviceContext* aContext = myCtx->myDX11->GetContext();
    if (it == TextureID(-1))
    {
        aContext->PSSetShaderResources(aSlot, 1, &myNullRSV);
        return;
    }

    aContext->VSSetShaderResources(aSlot, 1, myTextures.GetByIterator(it).mySRV.GetAddressOf());
}

void ISTE::TextureManager::PsBindTexture(TextureID it, unsigned int aSlot)
{
    ID3D11DeviceContext* aContext = myCtx->myDX11->GetContext(); 
    if (it == TextureID(-1))
    {
        aContext->PSSetShaderResources(aSlot, 1, &myNullRSV);
        return; 
    }

    aContext->PSSetShaderResources(aSlot, 1, myTextures.GetByIterator(it).mySRV.GetAddressOf());
}

ISTE::TextureManager::TextureLoadResult ISTE::TextureManager::LoadTexture(std::wstring aTexturePath, bool aSrgbFlag, bool aMipMapFlag)
{
    if (!myTextures.Exists(aTexturePath))
        return LoadNewTexture(aTexturePath.c_str(), aSrgbFlag, aMipMapFlag);
    else 
        return { true, myTextures.Get(aTexturePath).myIndex };
} 
FontID ISTE::TextureManager::LoadFont(std::string aTexturePath, size_t aFontSize, size_t aFontBorderSize)
{
    FontID res = myTextServicer->CheckIfExists(aTexturePath, aFontSize, aFontBorderSize);
    if (res == FontID(-1))
        return LoadNewFont(aTexturePath.c_str(), aFontSize, aFontBorderSize);
    else
        return res;
}


ISTE::TextureManager::TextureLoadResult ISTE::TextureManager::LoadNewTexture(const wchar_t* aFilePath, bool aSrgbFlag, bool aMipMapFlag)
{
    ID3D11Device* device = myCtx->myDX11->GetDevice();
    ID3D11DeviceContext* context = myCtx->myDX11->GetContext();

    //reads the dds/wic file and gets resource
    //note that it will always generate mipmaps right now
    ComPtr<ID3D11ShaderResourceView> resource;
    if (FAILED(ReadDDSTexture(aFilePath, resource, aSrgbFlag, aMipMapFlag)))
        if (FAILED(ReadWICTexture(aFilePath, resource, aSrgbFlag, aMipMapFlag)))
        {
#ifdef _DEBUG
            std::wstring msg = aFilePath;
            msg = L"Texture Manager    -   " + msg + L": Invalid Format - must be DDS, alternatively PNG, JPEG, BMP, TIFF or GIFF";
            std::string utf8Msg = StringCast<std::string>(msg);

            std::cout << utf8Msg << std::endl;
            Logger::DebugOutputError(msg);
#endif
            return {false, LoadTexture(L"../Assets/Sprites/ERROR.dds",true, false)};
        }
        
    //generates MipMaps 
    if (aMipMapFlag)
        context->GenerateMips(resource.Get());


    //gets height and width from resource
    ID3D11Resource* res = nullptr;
    resource->GetResource(&res);
    ID3D11Texture2D* textureRes = (ID3D11Texture2D*)res;
    res->Release();

    D3D11_TEXTURE2D_DESC desc;
    textureRes->GetDesc(&desc);

    unsigned int x = desc.Width;
    unsigned int y = desc.Width; 
    // power of two check
    {
    bool powerOfTwo = !(x == 0) && !(x & (x - 1));
    powerOfTwo &= !(y == 0) && !(y & (y - 1));
#ifdef _DEBUG
    if (!powerOfTwo)
    { 
        std::wstring msg = aFilePath;
        msg = L"Texture Manager    -   " + msg + L" " + std::to_wstring(x) + L"x" + std::to_wstring(y) + L": IS NOT POWER OF TWO!!!!";
        std::string utf8Msg = StringCast<std::string>(msg);
        std::cout << utf8Msg << std::endl; 
    }
#endif
    }


    //inserts info from resource
    Texture createdTexture;
    createdTexture.myTexSize = { x, y };
    createdTexture.mySRV = resource;
    createdTexture.myFilePath = aFilePath;


    //inserts and returns 
    TextureID index = myTextures.Insert(aFilePath, createdTexture);

    myTextures.GetByIterator(index).myIndex = index;
    return { true, index };
}

FontID ISTE::TextureManager::LoadNewFont(const char* aFilePath, size_t aFontSize, size_t aFontBorderSize)
{
    ID3D11Device* device = myCtx->myDX11->GetDevice();
    ID3D11DeviceContext* context = myCtx->myDX11->GetContext();
    std::string path = aFilePath;
    TextService::FontLoadResult res = myTextServicer->LoadFont(path, aFontSize, aFontBorderSize);
    if (!res.mySuccessFlag)
    {
        path = "Failed to load font: " + path;
        Logger::CoutError(path);
        return FontID(-1);
    }

    Texture createdTexture;
    createdTexture.myFilePath   = StringCast<std::wstring>(aFilePath);
    createdTexture.mySRV        = res.myFontTexture; 

    std::wstring key = StringCast<std::wstring>(aFilePath);
    key = key + L"|" + std::to_wstring(aFontSize) + L"|" + std::to_wstring(aFontBorderSize);
    TextureID id = myTextures.Insert(key, createdTexture);

    myTextServicer->GetFontData(res.myFontID)->myFontAtlasID = id;
    return res.myFontID;
}
 

HRESULT ISTE::TextureManager::ReadWICTexture(const wchar_t* aFilePath, ComPtr<ID3D11ShaderResourceView>& aSRV, bool aSrgbFlag, bool aMipMapFlag)
{
    //my fucking god
    HRESULT result;
    if (aSrgbFlag)
    {
        if (aMipMapFlag)
        {
            result = DirectX::CreateWICTextureFromFileEx(myCtx->myDX11->GetDevice(), myCtx->myDX11->GetContext(), aFilePath,
                0, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET, 0, D3D11_RESOURCE_MISC_GENERATE_MIPS,
                DirectX::WIC_LOADER_FORCE_SRGB, nullptr, aSRV.GetAddressOf());
        }
        else
        {
            result = DirectX::CreateWICTextureFromFileEx(myCtx->myDX11->GetDevice(), myCtx->myDX11->GetContext(), aFilePath,
                0, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET, 0, 0,
                DirectX::WIC_LOADER_FORCE_SRGB, nullptr, aSRV.GetAddressOf());
        }
    }
    else
    {
        if (aMipMapFlag)
        {
            result = DirectX::CreateWICTextureFromFileEx(myCtx->myDX11->GetDevice(), myCtx->myDX11->GetContext(), aFilePath,
                0, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET, 0, D3D11_RESOURCE_MISC_GENERATE_MIPS,
                DirectX::WIC_LOADER_DEFAULT, nullptr, aSRV.GetAddressOf());
        }
        else
        {
            result = DirectX::CreateWICTextureFromFileEx(myCtx->myDX11->GetDevice(), myCtx->myDX11->GetContext(), aFilePath,
                0, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET, 0, 0,
                DirectX::WIC_LOADER_DEFAULT, nullptr, aSRV.GetAddressOf());
        }
    }

    return result;
}

HRESULT ISTE::TextureManager::ReadDDSTexture(const wchar_t* aFilePath, ComPtr<ID3D11ShaderResourceView>& aSRV, bool aSrgbFlag, bool aMipMapFlag)
{
    HRESULT result;
    if (aMipMapFlag)
    {
        result = DirectX::CreateDDSTextureFromFileEx(myCtx->myDX11->GetDevice(), myCtx->myDX11->GetContext(), aFilePath,
            0, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET, 0, D3D11_RESOURCE_MISC_GENERATE_MIPS,
            aSrgbFlag, nullptr, aSRV.GetAddressOf());
    }
    else
    {
        result = DirectX::CreateDDSTextureFromFileEx(myCtx->myDX11->GetDevice(), myCtx->myDX11->GetContext(), aFilePath,
            0, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, 0,
            aSrgbFlag, nullptr, aSRV.GetAddressOf());
    }
    return result; 
}