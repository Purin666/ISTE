#include "../PostprocessStructs.hlsli"
#include "../Common.hlsli"
cbuffer MotionBlurData : register(b2)
{
    float   myRedOffset = 0;
    float   myGreenOffset = 0;
    float   myBlueOffset = 0;
    int     myType = 0;

    float2  myMouseCoord;
    float2  garb;
};

float4 main(PostProcessVertexToPixel input) : SV_Target0
{
    float2 direction = input.myPosition.xy - myMouseCoord;
    direction /= myResolution;
    float r = FullscreenTexture.Sample(DefaultSampler, input.myUV + (direction * float2(myRedOffset, myRedOffset))).r;
    float g = FullscreenTexture.Sample(DefaultSampler, input.myUV + (direction * float2(myGreenOffset, myGreenOffset))).g;
    float b = FullscreenTexture.Sample(DefaultSampler, input.myUV + (direction * float2(myBlueOffset, myBlueOffset))).b;
    
    return float4(r, g, b, 1.0f);
}