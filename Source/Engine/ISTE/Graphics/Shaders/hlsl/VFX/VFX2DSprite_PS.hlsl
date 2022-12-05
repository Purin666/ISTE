#include "../Common.hlsli"
#include "../PBRFunctions.hlsli"

cbuffer CommonObjbuffer : register(b2)
{
    float3 myColor;
    int myObjId;
};

ColorAndEntityOutput main(PixelInputType anInput)
{ 
    float4 radiance = AlbedoTex.Sample(DefaultSampler, anInput.myUvCoordinates.xy).rgba;
    radiance.rgb *= myColor; 
    
    ColorAndEntityOutput outdata;
    outdata.myAlbedo = radiance;
    outdata.myEntities = 0;
    return outdata;
}