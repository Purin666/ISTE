#include "../Common.hlsli"
#include "../PBRFunctions.hlsli"

cbuffer ObjectBuffer : register(b2)
{
    float2 myPosition;
    float2 myScale;
    float1 myRotation;
    float1 garb;
    float2 myPivot;
    
    float2 myUVStart;
    float2 myUVEnd;
    float2 myUVOffset;
    float2 myUVScale;
    
    float4 myColor;
};  

ColorAndEntityOutput main(PixelInputType anInput)
{ 
    ColorAndEntityOutput outdata;
    float4 radiance = AlbedoTex.Sample(DefaultSampler, anInput.myUvCoordinates.xy).rgba; 
    radiance *= myColor;
    
    outdata.myAlbedo = radiance;
    outdata.myEntities = 0;
    
    return outdata;
}