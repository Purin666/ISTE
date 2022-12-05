#include "../Common.hlsli"

cbuffer ObjectBuffer : register(b2)
{
    float4x4 myModelTransform;
    float4 myColorAndIntensity;
    float myRange;
    float2 myLightPerspectiveValues;
    float garb;
};
//

float4 main(VertexInputType aVertexInput) : SV_POSITION
{ 
    float4x4 modelTransform = myModelTransform; 
    
    float4 myPosition = mul(modelTransform, float4(aVertexInput.myPosition, 1));
           myPosition = mul(myWorldToClipMatrix, myPosition);
    return myPosition;

}