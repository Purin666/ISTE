#pragma once
#include <wrl/client.h>  
#include "ISTE/Graphics/RenderTarget.h"
#include "ISTE/Graphics/DepthStencilTarget.h"
#include "ISTE/Graphics/RenderDefines.h"
#include "ISTE/Math/Vec4.h"
#include "ISTE/Math/Matrix4x4.h"

using Microsoft::WRL::ComPtr;
struct ID3D11Buffer;
struct ID3D11PixelShader;
struct ID3D11VertexShader;
struct ID3D11InputLayout;

namespace ISTE {
    struct Context;
    class SSAOEffect {
    public:
        struct SSAOBufferData {
            CU::Matrix4x4f  myWorldToViewINV;
            float           mySampleRadius    = 1.0f;
            float           myKernalSize      = 64.f;
            float           buffer[2];
            CU::Vec4f       myKernals[MAX_SSAO_KERNAL_SIZE];
        };

        bool Init();
        void Draw();

        SSAOBufferData& GetBufferData   () { return mySSAOBufferData; }
        RenderTarget&   GetRenderTarget () { return mySSAOTarget; }

    private: //shader stuff.
        friend class DX11;
        Context*                myCtx;
        ComPtr<ID3D11Buffer>    mySSAOBuffer; 
        SSAOBufferData          mySSAOBufferData;
        RenderTarget            mySSAOTarget;
        DepthStencilTarget      mySSAODepthTarget;

        void GenerateKernals();
        void BindBuffers    ();
        bool InitBuffers    (); 
    };
};