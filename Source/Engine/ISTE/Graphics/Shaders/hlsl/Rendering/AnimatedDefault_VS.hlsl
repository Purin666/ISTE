#include "../Common.hlsli"

cbuffer ObjectBuffer : register(b2)
{
    float4x4 myModelTransform;
    float4x4 myPrevModelTransform;
    float4x4 myBones[MaxBoneCount];
}; 

PixelInputType main(VertexInputType aVertexInput)
{
    PixelInputType output;
    
    float4x4 skinnedTransform = 0; 
    
    //adds the influences of each bone
    float wheight = aVertexInput.myWeights.x;
    skinnedTransform += wheight * myBones[aVertexInput.myBones.x];
    
    wheight = aVertexInput.myWeights.y;
    skinnedTransform += wheight * myBones[aVertexInput.myBones.y];
    
    wheight = aVertexInput.myWeights.z;
    skinnedTransform += wheight * myBones[aVertexInput.myBones.z];
    
    wheight = aVertexInput.myWeights.w;
    skinnedTransform += wheight * myBones[aVertexInput.myBones.w];
    
    //transforms it to the right space    
    float4x4 toWorld        = mul(myModelTransform, skinnedTransform);
    float4x4 toWorldPrev    = mul(myPrevModelTransform, skinnedTransform);
     
    output.myPosition   = mul(toWorld,      float4(aVertexInput.myPosition, 1));
    output.myPrevPos    = mul(toWorldPrev,  float4(aVertexInput.myPosition, 1));
    
    output.myWorldPosition  = output.myPosition.xyz;
    output.myPosition       = mul(myWorldToClipMatrix, output.myPosition); 
    output.myPrevPos        = mul(myPrevWorldToClipMatrix, output.myPrevPos);
     
    
    //Model normals
    float3x3 skinnedRotation = (float3x3) toWorld;
    //fråga ifall det går att göra deta eller ifall man måste ta inversen av båda matriserna innan man mul-ar dem
    output.myNormal = mul(transpose(invertMatrix(skinnedRotation)), aVertexInput.myNormal);
    output.myNormal = normalize(output.myNormal);
    
    output.myBiNormal = mul(skinnedRotation, aVertexInput.myBiNormal);
    output.myBiNormal = normalize(output.myBiNormal);
    
    output.myTangent = mul(skinnedRotation, aVertexInput.myTangent);
    output.myTangent = normalize(output.myTangent);
    
    //uv coords
    output.myUvCoordinates = aVertexInput.myUvCoordinates;

    //vertex color
    output.myColor = aVertexInput.myColor;
    return output;
}