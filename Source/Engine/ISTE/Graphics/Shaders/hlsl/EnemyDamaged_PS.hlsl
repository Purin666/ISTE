#include "Common.hlsli"
#include "PBRFunctions.hlsli"

ColorAndEntityOutput main(PixelInputType aPixelInputType)
{
    //towards viewposition
    float3 toEye = normalize(myCamPosition - aPixelInputType.myWorldPosition);
    
    //GATHERS DATA FROM NORMAL MAP 
    float4 normalTexture = NormalTex.Sample(DefaultSampler, aPixelInputType.myUvCoordinates.xy);
    float ambientOcclusion = normalTexture.b;
    
    float3 expandedNormal = expandNormal(normalTexture);
    
    //Transforms normal to pixel
    float3x3 tbnMatrix = float3x3(
        aPixelInputType.myTangent,
        aPixelInputType.myBiNormal,
        aPixelInputType.myNormal
    );
    
    float3 pixelNormal = normalize(mul(expandedNormal, tbnMatrix));
    //return float4(pixelNormal, 1);
    
    //GATHERS DATA FROM ALBEDO
    float4 albedo = AlbedoTex.Sample(DefaultSampler, aPixelInputType.myUvCoordinates.xy).rgba;
    
    //GATHERS DATA FROM MATERIALMAP
    float4 materialMap = MaterialTex.Sample(DefaultSampler, aPixelInputType.myUvCoordinates.xy);
    
    float metalness = materialMap.r;
    float roughness = materialMap.g;
    float emissive = materialMap.b;
    float emissiveStr = materialMap.a;
    
    //light calculations
    float3 specularColor = lerp((float3) 0.04f, albedo.rgb, metalness);
    float3 diffuseColor = lerp((float3) 0.00f, albedo.rgb, 1 - metalness);
    
    float3 ambiance = EvaluateAmbiance(CubeMap, pixelNormal, aPixelInputType.myNormal, toEye.xyz,
        roughness, ambientOcclusion, diffuseColor, specularColor
    );
    
    float3 directionalLight = EvaluateDirectionalLight(diffuseColor, specularColor, pixelNormal, roughness,
        (myDirectionalLightColor.rgb * myDirectionalLightColor.a), myDirectionalLightDir, toEye.xyz
    );
    
    float3 emmissiveAlbedo = albedo.rgb * emissive * emissiveStr;
    float3 ambientLight = ((0.5f + 0.5f * pixelNormal.y) * (mySkyColor.rgb * mySkyColor.a)) + ((0.5f - 0.5f * pixelNormal.y) * (myGroundColor.rgb * myGroundColor.a));
    
    //float3(0,0,0); //
    float3 radiance = ambientLight + emmissiveAlbedo + ambiance + directionalLight;

    float4 white = { 1.0f, 1.0f, 1.0f, 1.0f };
    float elapsedTime = myTimings.y;
    float4 whiteScale = white * (sin(elapsedTime * 20) / 2 + 1);
    radiance *= whiteScale;
    
    ColorAndEntityOutput outdata;
    outdata.myAlbedo = float4(radiance,1);
    outdata.myEntities = 0;
    return outdata;
}