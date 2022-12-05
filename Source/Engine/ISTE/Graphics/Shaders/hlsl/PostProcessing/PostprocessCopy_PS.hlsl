#include "../CommonPP.hlsli"

PostProcessPixelOutput main(PostProcessVertexToPixel input)
{
	PostProcessPixelOutput returnValue;
    returnValue.myColor.rgb = FullscreenTexture.Sample(DefaultPPSampler, input.myUV).rgb;
	returnValue.myColor.a = 1.0f;
	return returnValue;
}