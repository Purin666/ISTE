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

PixelInputType main(VertexInputType aVertexInput)
{
    PixelInputType output;
    output.myColor = aVertexInput.myColor;
    output.myPosition = float4(aVertexInput.myPosition, 1);
    
    //transforms it to the right space   
    output.myPosition = mul(myModelTransform, output.myPosition);
    output.myPosition = mul(myWorldToClipMatrix, output.myPosition);
    
    //uv coords
    output.myUvCoordinates = GetUVRect(float4(myUVStart, myUVEnd), aVertexInput.myVertexIndex);
    output.myUvCoordinates *= myUVScale;
    output.myUvCoordinates += myUVOffset;
    
    //Model normals
    float3x3 modelToWorld3x3 = (float3x3) myModelTransform;
    output.myNormal = mul(modelToWorld3x3, aVertexInput.myNormal);
    output.myNormal = normalize(output.myNormal);
    
    output.myBiNormal = mul(modelToWorld3x3, aVertexInput.myBiNormal);
    output.myBiNormal = normalize(output.myBiNormal);
    
    output.myTangent = mul(modelToWorld3x3, aVertexInput.myTangent);
    output.myTangent = normalize(output.myTangent);
    
    output.myWorldPosition = mul(myModelTransform, float4(aVertexInput.myPosition, 1)).xyz;
    return output;
}