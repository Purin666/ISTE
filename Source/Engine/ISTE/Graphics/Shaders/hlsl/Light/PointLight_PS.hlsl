#include "../Common.hlsli"
#include "../PBRFunctions.hlsli"


cbuffer ObjectBuffer : register(b2)
{
    float4x4 myModelTransform;
    float4 myColorAndIntensity;
    float myRange;
    float2 myLightPerspectiveValues;
    float garb;
}; 

float4 main(float4 aPosition : SV_Position) : SV_TARGET
{
    float2 uv = aPosition.xy / myResolution.xy;
    
    float4 worldPos = GBufferPosition.Sample(DefaultSampler, uv).rgba;
    float4 albedo   = GBufferAlbedo.Sample(DefaultSampler, uv).rgba;
    float4 pNormal  = GBufferPixelNormal.Sample(DefaultSampler, uv).rgba;
    float4 material = GBufferMaterial.Sample(DefaultSampler, uv).rgba;
     
    pNormal.xyz = normalize((pNormal.xyz * 2.f) - 1.f);
	
	
    float3 pointLightPos = float3(
                    myModelTransform[0].w,
                    myModelTransform[1].w,
                    myModelTransform[2].w
                    );
    
    float3 pointLightSize = length(myModelTransform[0].xyz);
                    
        
    float3 toEye = normalize(myCamPosition - worldPos.xyz);
    float3 specularColor = lerp((float3) 0.04f, albedo.rgb, material.r);
    float3 diffuseColor = lerp((float3) 0.00f, albedo.rgb, 1 - material.r);
    
    float3 pointLights = EvaluatePointLight(
			diffuseColor, specularColor, pNormal.xyz, material.g,
			myColorAndIntensity.rgb, myColorAndIntensity.w, myRange, pointLightPos,
			toEye.xyz, worldPos.xyz);
 
    return float4(pointLights, 1);
}