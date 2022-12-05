#include "../Common.hlsli"

float4 main(PixelInputType aPixelInput) : SV_TARGET
{
    return AlbedoTex.Sample(DefaultSampler, aPixelInput.myUvCoordinates).rgba;
}