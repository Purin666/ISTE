#include "../Common.hlsli"
#include "../PBRFunctions.hlsli"

float3 s_curve(float3 x)
{
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
    return clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0, 1.0);
}

float3 tonemap_s_gamut3_cine(float3 c)
{
    // based on Sony's s gamut3 cine
    float3x3 fromSrgb = float3x3(
        +0.6456794776, +0.2591145470, +0.0952059754,
        +0.0875299915, +0.7596995626, +0.1527704459,
        +0.0369574199, +0.1292809048, +0.8337616753);

    float3x3 toSrgb = float3x3(
        +1.6269474099, -0.5401385388, -0.0868088707,
        -0.1785155272, +1.4179409274, -0.2394254004,
        +0.0444361150, -0.1959199662, +1.2403560812);

    return mul(toSrgb, s_curve(mul(fromSrgb, c)));
}

float4 PPToneMap(float4 input)
{
    float4 returnValue;
    returnValue.rgb = tonemap_s_gamut3_cine(input.rgb);
    returnValue.a = 1.0f;
    return returnValue;
}



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
    
    float metalness     = materialMap.r;
    float roughness     = materialMap.g; 
    float emissive      = materialMap.b; 
    float emissiveStr   = materialMap.a;
    
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
    float4 result = float4(radiance, albedo.a);
    
    ColorAndEntityOutput outdata;
    outdata.myAlbedo = result;
    outdata.myEntities = 0;
    return outdata;
} 