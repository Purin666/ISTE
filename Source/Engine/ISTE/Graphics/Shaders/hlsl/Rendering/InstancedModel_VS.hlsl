#include "../Common.hlsli"

InstancedPixelInputType main(InstancedVertexInput aVertexInput)
{
    InstancedPixelInputType output;
    output.myColor = aVertexInput.myColor;
    output.myPosition = float4(aVertexInput.myPosition, 1);
    
    //transforms it to the right space   
    output.myWorldPosition = output.myPosition = mul(aVertexInput.myWorldTransform, output.myPosition);
    output.myPosition = mul(myWorldToClipMatrix, output.myPosition);
    
    //uv coords
    output.myUvCoordinates = (aVertexInput.myUvCoordinates + aVertexInput.myScaleAndOffset.zw) * aVertexInput.myScaleAndOffset.xy;
    
    //Model normals
    float3x3 modelToWorld3x3 = (float3x3) aVertexInput.myWorldTransform;
    output.myNormal = mul(modelToWorld3x3, aVertexInput.myNormal);
    output.myNormal = normalize(output.myNormal);
    
    output.myBiNormal = mul(modelToWorld3x3, aVertexInput.myBiNormal);
    output.myBiNormal = normalize(output.myBiNormal);
    
    output.myTangent = mul(modelToWorld3x3, aVertexInput.myTangent);
    output.myTangent = normalize(output.myTangent);
    
    output.myIDs.r = aVertexInput.myVertexID;
    output.myIDs.g = aVertexInput.myInstanceID;

    return output;
}