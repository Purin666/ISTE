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


static float3 SampleOffsetDirections[20] =
{
    float3(1, 1, 1), float3(1, -1, 1), float3(-1, -1, 1), float3(-1, 1, 1),
        float3(1, 1, -1), float3(1, -1, -1), float3(-1, -1, -1), float3(-1, 1, -1),
        float3(1, 1, 0), float3(1, -1, 0), float3(-1, -1, 0), float3(-1, 1, 0),
        float3(1, 0, 1), float3(-1, 0, 1), float3(1, 0, -1), float3(-1, 0, -1),
        float3(0, 1, 1), float3(0, -1, 1), float3(0, -1, -1), float3(0, 1, -1)
};


float4 main(float4 aPosition : SV_Position) : SV_TARGET
{
    float2 uv = aPosition.xy / myResolution.xy;
    
    float4 worldPos = GBufferPosition.Sample(DefaultSampler, uv).rgba;
    float4 albedo = GBufferAlbedo.Sample(DefaultSampler, uv).rgba;
    float4 pNormal = GBufferPixelNormal.Sample(DefaultSampler, uv).rgba;
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
    
    float3 pointLightColor = EvaluatePointLight(
			diffuseColor, specularColor, pNormal.xyz, material.g,
			myColorAndIntensity.rgb, myColorAndIntensity.w, myRange, pointLightPos,
			toEye.xyz, worldPos.xyz);
          
    float3 pixelToLight = worldPos.xyz - pointLightPos;
    
    float shadowFactor = 0; 
    float texelOffset = 1.f / 250.f;
    float offsetScale = 4; //float2(ddx(uv.x), ddy(uv.y)); 
    //int n = 2; 
    
   //for (int i = 0; i < 20; i++)
   //{
        //float3 offset = SampleOffsetDirections[i] * texelOffset * 4;
        float3 ToPixelAbs = abs(pixelToLight);
        float Z = max(ToPixelAbs.x, max(ToPixelAbs.y, ToPixelAbs.z));
        float computedZ = (myLightPerspectiveValues.x * Z + myLightPerspectiveValues.y) / Z;
        float shadowMapZ = ShadowCubes.Sample(ShadowSampler, pixelToLight);
        shadowFactor = (computedZ < (shadowMapZ));
    //}
    
   // for (int i = 0; i < 20; i++)
   // {
   //     float3 offset = SampleOffsetDirections[i] * texelOffset * offsetScale;
   //     float3 TempToLight = worldPos.xyz + offset - pointLightPos;
   //     float3 ToPixelAbs = abs(pixelToLight);
   //     float Z = max(ToPixelAbs.x, max(ToPixelAbs.y, ToPixelAbs.z));
   //     float computedZ = (myLightPerspectiveValues.x * Z + myLightPerspectiveValues.y) / Z;
   //     float shadowMapZ = ShadowCubes.Sample(ShadowSampler, TempToLight);
   //     shadowFactor += (computedZ < (shadowMapZ));
   // }
    //
    //shadowFactor /= 20;
    return float4(pointLightColor * shadowFactor, 1);
}