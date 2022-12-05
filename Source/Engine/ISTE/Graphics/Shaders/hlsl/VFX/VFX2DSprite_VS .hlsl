#include "Common.hlsli"

cbuffer ObjectBuffer : register(b2)
{
    float2 myPosition;
    float2 myScale;
    float1 myRotation;
    float3 garb;
    
    float2 myUVStart;
    float2 myUVEnd;
    float2 myUVOffset;
    float2 myUVScale; 
};  

PixelInputType main(VertexInputType input)
{
    PixelInputType output;
    output.myPosition = 0;
    output.myPosition.w = 1;
    
    //manual 2d transform
    float newX = input.myPosition.x * cos(myRotation) - input.myPosition.y * sin(myRotation);
    float newY = input.myPosition.x * sin(myRotation) + input.myPosition.y * cos(myRotation); 
    
    newX *= myScale.x;
    newY *= myScale.y;
    
    output.myWorldPosition.xy = 0;
    output.myWorldPosition.xy = output.myPosition.xy = float2(newX, newY); 
    //manual 2d transform
 
    output.myNormal = input.myNormal;
    output.myTangent = input.myTangent;
    output.myBiNormal = input.myBiNormal; 
    output.myColor = input.myColor;  
    output.myUvCoordinates = GetUVRect(float4(myUVStart, myUVEnd), input.myVertexIndex);
    output.myUvCoordinates *= myUVScale;
    output.myUvCoordinates += myUVOffset;
    
    return output;
}