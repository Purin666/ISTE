#include "../Common.hlsli" 

ColorAndEntityOutput main(PixelInputType input)
{
    float3x3 tbnMatrix = float3x3(
        input.myTangent,
        input.myBiNormal,
        input.myNormal
    );
    
    float4 normalTexture = NormalTex.Sample(DefaultSampler, input.myUvCoordinates.xy);
    float3 expandedNormal = expandNormal(normalTexture);
    float3 pixelNormal = normalize(mul(expandedNormal, tbnMatrix));
    
    ColorAndEntityOutput outdata;
    outdata.myAlbedo = float4(input.myNormal, 1);
    outdata.myEntities = 0;
    return outdata;

}