#include "../CommonPP.hlsli"
#include "../Common.hlsli"
cbuffer FogBuffer : register(b2)
{
    float3 myFogColor;
    float1 myFogStartDist;
    
    float3 myFogHighlightColor;
    float1 myFogGlobalDensity; 
    
    float1 myFogHeightFalloff;
    float3 garb;
};

float3 ApplyFog(float3 originalColor, float eyePosY, float3 eyeToPixel)
{
    float pixelDist = length(eyeToPixel);
    float3 eyeToPixelNorm = eyeToPixel / pixelDist;
    // Find the fog staring distance to pixel distance
    float fogDist = max(pixelDist - myFogStartDist, 0.0);
    // Distance based fog intensity
    float fogHeightDensityAtViewer = exp(-myFogHeightFalloff * eyePosY);
    float fogDistInt = fogDist * fogHeightDensityAtViewer;
    // Height based fog intensity
    float eyeToPixelY = eyeToPixel.y * (fogDist / pixelDist);
    float t = myFogHeightFalloff * eyeToPixelY;
    const float thresholdT = 0.01;
    float fogHeightInt = abs(t) > thresholdT ? (1.0 - exp(-t)) / t : 1.0;
    // Combine both factors to get the final factor
    float fogFinalFactor = exp(-myFogGlobalDensity * fogDistInt * fogHeightInt);
    // Find the sun highlight and use it to blend the fog color
    float sunHighlightFactor = saturate(dot(eyeToPixelNorm, myDirectionalLightDir));
    sunHighlightFactor = pow(sunHighlightFactor, 8.0);
    float3 fogFinalColor = lerp(myFogColor, myFogHighlightColor, sunHighlightFactor);
    return lerp(fogFinalColor, originalColor, fogFinalFactor);
}

/*
float3 ApplyFog(float3 originalColor, float pixelHeight, float eyePosY, float3 eyeToPixel)
{
    float pixelDist = length(eyeToPixel);
    float3 eyeToPixelNorm = eyeToPixel / pixelDist;
    
    // Find the fog staring distance to pixel distance
    float fogDist = max(pixelDist - myFogStartDist, 0.0);
    
    // Distance based fog intensity
    float fogDistInt = fogDist;
    
    // Height based fog intensity
    float threshold = 4;
    
    float fogHeightDensityAtViewer = exp(-myFogHeightFalloff);
    float eyeToPixelY = eyePosY * (fogDist / pixelDist);
    float t = log(max(0, (pixelHeight)) + 1) * max(myFogHeightFalloff * threshold, 0.001);
    const float thresholdT = 0.01;
    float fogUpperHeightInt = abs(t) > thresholdT ? max(threshold - t, 0) : 1.0;
    
    
    float fogLowerHeightInt = abs(t) > thresholdT ? max(threshold - t, 0) : 1.0;
    
    float heightFogDensity = lerp(fogLowerHeightInt, fogUpperHeightInt, ) * fogHeightDensityAtViewer;
    
    //saturate(log(eyePosY) * (1 - myFogHeightUpperFalloff))
    
    // Combine both factors to get the final factor
    float fogFinalFactor = exp(-myFogGlobalDensity * fogDistInt * fogHeightInt);
    
    // Find the sun highlight and use it to blend the fog color
    float sunHighlightFactor = saturate(dot(eyeToPixelNorm, myDirectionalLightDir));
    sunHighlightFactor = pow(sunHighlightFactor, 8.0);
    float3 fogFinalColor = lerp(myFogColor, myFogHighlightColor, sunHighlightFactor);
    return lerp(fogFinalColor, originalColor, fogFinalFactor);
}
*/

float4 main(PostProcessVertexToPixel anInput) : SV_Target
{
    float4 worldPos = 0;
    float3 resource = FullscreenTexture.Sample(DefaultSampler, anInput.myUV.xy).rgb;
    worldPos = GBufferPosition.Sample(DefaultSampler, anInput.myUV.xy).rgba; 
   
    if (worldPos.x == 0)
        worldPos.x = 500;
    if (worldPos.y == 0)
        worldPos.y = 100;
    if (worldPos.z == 0)
        worldPos.z = 500;
    
    float3 toEye = myCamPosition - worldPos.xyz;
    
    return float4(ApplyFog(resource, myCamPosition.y, toEye), 1);
}