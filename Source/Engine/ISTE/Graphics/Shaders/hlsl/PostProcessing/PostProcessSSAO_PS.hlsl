#include "../PostprocessStructs.hlsli"
#include "../Common.hlsli"

//FullscreenTexture     -       depth texture of what is visible to the main camera. ie the depth prepass

//should be sett once and never again honestly
cbuffer SSAOData : register(b2)
{
    float4x4 myTempWorldToClip;
    float1 mySampleRadius;
    float1 myKernalSize;
    float2 garb;
    float4 myKernals[MaxSSAOKernalSize];
};


float main(PostProcessVertexToPixel input) : SV_Target0
{
    PostProcessPixelOutput returnValue;
    returnValue.myColor = float4(1, 1, 1, 1);
    
    float3      pos         = GBufferPosition.Sample(    GBufferSampler , input.myUV).rgb;       //should just be gathered from depth  
    float3      originDepth = FullscreenTexture.Sample(  GBufferSampler , input.myUV).r;
    float3      normal      = GBufferPixelNormal.Sample(GBufferSampler , input.myUV).rgb;
    
    if (length(normal) == 0)
        return returnValue.myColor.r; 
    
                normal      = normalize((normal * 2.f) - 1.f);
    float3      tangent     = normalize(cross(normal, float3(0,0,1)));
    float3      bitangent   = normalize(cross(normal, float3(1,0,0)));
    float3x3    tbn         = float3x3(tangent, bitangent, normal);
    
    
    float4      dSample     = float4(0,0,0,0);
    float       occlusion   = 0;
    float       sampleDepth = 0;
    float       rangeCheck  = 0; 
    float3      samplePos   = 0;
    float       computedZ   = 0;
    int         i           = 0;
    
    for (; i < myKernalSize; i++)
    {
        // Get sample z pos
        dSample.xyz = mul(myKernals[i].xyz, tbn) * mySampleRadius;
        dSample.xyz += pos.xyz;
        dSample.w = 1.f;

        dSample = mul(myWorldToClipMatrix, dSample);
        dSample.xyz /= dSample.w;
        computedZ = dSample.z;
        dSample.y *= -1;
        dSample.xy = dSample.xy * 0.5 + 0.5;
        
        // Get sample depth
        sampleDepth = FullscreenTexture.Sample(GBufferSampler, dSample.xy).r;
        samplePos   = GBufferPosition.Sample(GBufferSampler, dSample.xy).rgb;
        
        // range check & accumulate
        rangeCheck = smoothstep(0.f, 1.f, 1.f / length(pos - samplePos));
        //abs(originDepth - sampleDepth) < mySampleRadius / 50.f ? 1.0 : 0.0;
        occlusion += (sampleDepth <= computedZ) * rangeCheck;
    }
    
    returnValue.myColor.xyz = 1 - (occlusion / myKernalSize);
    returnValue.myColor.w = 1;
    return returnValue.myColor.r;
}