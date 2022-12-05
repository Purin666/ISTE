#include "../PostprocessStructs.hlsli"
#include "../Common.hlsli"



cbuffer MotionBlurData : register(b2)
{ 
    float1  myBlurStrength;
    float   myKernalSize;
    float2  garb;
};

float4 main(PostProcessVertexToPixel input) : SV_Target0
{  
    float3  col         = float3(0.f, 0.f, 0.f);
    float2  offset      = float2(0.f, 0.f);
    int     kernalSize  = myKernalSize;
    float2  uv          = input.myUV ;
    float2  NDCuv       = uv.xy * 2.f - 1.f;
    float1  pxlLength   = length(NDCuv) / (1.2f * 250.f); 
    
    pxlLength *= myBlurStrength;
    for (int i = 0; i < kernalSize; i++)
    {
        col     += FullscreenTexture.Sample(GBufferSampler, uv + NDCuv * offset).rgb;
        offset  += pxlLength;
    }
    
    return float4(col.xyz / (float)kernalSize, 1.f);
}