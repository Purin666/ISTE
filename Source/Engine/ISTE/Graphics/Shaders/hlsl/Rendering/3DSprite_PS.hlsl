#include "../Common.hlsli"

cbuffer ObjectBuffer : register(b2)
{
    float4x4 myModelTransform;
    float myEntitiyID;
    float3 garb;
    float4 myColor;
    
    float2 myUVStart;
    float2 myUVEnd;
    float2 myUVOffset;
    float2 myUVScale;
};

ColorAndEntityOutput main(PixelInputType aPixelInput)
{
    ColorAndEntityOutput outdata;
    float4 radiance = AlbedoTex.Sample(DefaultSampler, aPixelInput.myUvCoordinates.xy).rgba;
    if (radiance.a < 0.2)
        discard;
    
    outdata.myAlbedo = radiance * myColor;
    outdata.myEntities = myEntitiyID + 1;

    return outdata;
}