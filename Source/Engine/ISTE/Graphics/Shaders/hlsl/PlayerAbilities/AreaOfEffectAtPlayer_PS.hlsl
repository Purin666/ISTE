#include "../Common.hlsli"
#include "../PBRFunctions.hlsli"

float2 Rotate(float2 aVec, float aRadian)
{
    float2 result = aVec;
    
    result.x = aVec.x * cos(aRadian) - aVec.y * sin(aRadian);
    result.y = aVec.x * sin(aRadian) + aVec.y * cos(aRadian);
    
    return result;
}

ColorAndEntityOutput main(VFXPixelInputType aPixelInputType)
{
    float2 uv = aPixelInputType.myUvCoordinates.xy;
    //uv.x += myTimings.y * 0.2f;
    //uv.x += sin(myTimings.y) * 0.1f + 0.5f;
    //uv.y += myTimings.y * -0.5f;
    
    //testing //Lukas

    float uvY = uv.y;
    uv = Rotate(uv, myTimings.y);
    uv.y = uvY;
    
    //towards viewposition
    float3 toEye = normalize(myCamPosition - aPixelInputType.myWorldPosition);
    
    //GATHERS DATA FROM NORMAL MAP 
    float4 normalTexture = NormalTex.Sample(DefaultSampler, uv);
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
    float4 albedo = AlbedoTex.Sample(DefaultSampler, uv).rgba;
    
    //GATHERS DATA FROM MATERIALMAP
    float4 materialMap = MaterialTex.Sample(DefaultSampler, uv);
    
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

    //float4 red = { 1.0f, 0.0f, 0.0f, 1.0f };
    //float elapsedTime = myTimings.y;
    //float4 redScale = red * (sin(elapsedTime * 20) / 2 + 1);
    //radiance *= redScale;
    
    ColorAndEntityOutput outdata;
    float4 result = float4(radiance, albedo.a);
    outdata.myAlbedo = result;
    outdata.myEntities = -1;

    return outdata;
}