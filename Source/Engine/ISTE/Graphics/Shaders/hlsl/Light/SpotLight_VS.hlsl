#include "../Common.hlsli"

cbuffer ObjectBuffer : register(b2)
{
    float4x4 myLightProjection;
    float4x4 myModelTransform;
    float4 myColorAndIntensity;
    float3 myDirection;
    float1 Garb1;
    
    float1 myRange;
    float1 myInnerLimit;
    float1 myOuterLimit;
    float1 garb2;
};

float4 main(VertexInputType aVertexInput) : SV_POSITION
{
    float4x4 modelTransform = myModelTransform;
    
    //fix clamping later
    //float3 pNorm = normalize(aVertexInput.myPosition);
    //float theThing = clamp(dot(myDirection, pNorm), (1 - myOuterLimit), 1);
            
    
    float4 myPosition = mul(modelTransform, float4(aVertexInput.myPosition, 1));
    myPosition = mul(myWorldToClipMatrix, myPosition);
    return myPosition;

}