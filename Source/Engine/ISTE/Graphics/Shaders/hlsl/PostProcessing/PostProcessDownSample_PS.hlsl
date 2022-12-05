#include "../CommonPP.hlsli"

PostProcessPixelOutput main(PostProcessVertexToPixel input)
{
    PostProcessPixelOutput returnValue;
	// ddx and ddy measures how much a parameter changes in x and y-direction per pixel
    float2 pixelOffset = float2(ddx(input.myUV.x), ddy(input.myUV.y));
	
	// Could have done one sample in the middle
	// But that results in some artifacts. This pattern gives a much smoother result
    float3 p00 = FullscreenTexture.Sample(DefaultPPSampler, input.myUV + pixelOffset * float2(-0.5f, -0.5f)).rgb;
    float3 p01 = FullscreenTexture.Sample(DefaultPPSampler, input.myUV + pixelOffset * float2(-0.5f, 0.5f)).rgb;
    float3 p10 = FullscreenTexture.Sample(DefaultPPSampler, input.myUV + pixelOffset * float2(0.5f, -0.5f)).rgb;
    float3 p11 = FullscreenTexture.Sample(DefaultPPSampler, input.myUV + pixelOffset * float2(0.5f, 0.5f)).rgb;
    returnValue.myColor.rgb = 0.25f * (p00 + p01 + p10 + p11);
    returnValue.myColor.a = 1.0f;
    return returnValue;
}