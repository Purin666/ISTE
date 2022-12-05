#include "../Common.hlsli"
#include "../PBRFunctions.hlsli"

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

float4 main(float4 aPosition : SV_Position) : SV_TARGET
{
    float2 uv = aPosition.xy / myResolution.xy;
    
    float4 worldPos = GBufferPosition.Sample(DefaultSampler, uv).rgba;
    float4 albedo = GBufferAlbedo.Sample(DefaultSampler, uv).rgba;
    float4 pNormal = GBufferPixelNormal.Sample(DefaultSampler, uv).rgba;
    float4 material = GBufferMaterial.Sample(DefaultSampler, uv).rgba; 
    
    pNormal.xyz = normalize((pNormal.xyz * 2.f) - 1.f);
    
    
    float3 spotLightPos = float3(
                    myModelTransform[0].w,
                    myModelTransform[1].w,
                    myModelTransform[2].w
                    );
    
    float3 toEye = normalize(myCamPosition - worldPos.xyz);
    float3 specularColor = lerp((float3) 0.04f, albedo.rgb, material.r);
    float3 diffuseColor = lerp((float3) 0.00f, albedo.rgb, 1 - material.r);
    
    float3 spotLightColor = EvaluateSpotLight(
			diffuseColor, specularColor, pNormal.xyz, material.g,
			myColorAndIntensity.rgb, myColorAndIntensity.w, myRange, spotLightPos.xyz,
			myDirection, myOuterLimit, myInnerLimit,
            toEye.xyz, worldPos.xyz
            ); 
    
    return float4(spotLightColor, 1);
    


}