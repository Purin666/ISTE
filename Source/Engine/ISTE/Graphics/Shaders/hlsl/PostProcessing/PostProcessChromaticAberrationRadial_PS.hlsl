#include "../PostprocessStructs.hlsli"
#include "../Common.hlsli"
cbuffer MotionBlurData : register(b2)
{
    float   myRedOffset = 0;
    float   myGreenOffset = 0;
    float   myBlueOffset = 0;
    int     myType = 0;

    float2  myMouseCoord;
    float2  myGeracg;
    float3  myRadialStregnth;
    float1  garb;
};

float4 main(PostProcessVertexToPixel input) : SV_Target0
{ 
    float2 uv = input.myUV;
    float2 NDCuv = ((uv.xy * 2.f) - 1.f);
    float R = FullscreenTexture.Sample(GBufferSampler, uv - NDCuv * myRadialStregnth.r * (length(NDCuv) / ((sin(myTimings.g * 4.5f) / 2.f + 1.f) * 100.f))).r;
    float G = FullscreenTexture.Sample(GBufferSampler, uv - NDCuv * myRadialStregnth.g * (length(NDCuv) / ((sin(myTimings.g * 4.5f) / 2.f + 1.f) * 100.f))).g;
    float B = FullscreenTexture.Sample(GBufferSampler, uv - NDCuv * myRadialStregnth.b * (length(NDCuv) / ((sin(myTimings.g * 4.5f) / 2.f + 1.f) * 100.f))).b;
    return float4(R,G,B,1);
}