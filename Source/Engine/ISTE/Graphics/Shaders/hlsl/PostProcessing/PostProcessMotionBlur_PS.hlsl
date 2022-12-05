#include "../PostprocessStructs.hlsli"
#include "../Common.hlsli"
cbuffer MotionBlurData : register(b2)
{
    float   mySamples = 10;
    float   myWeight;
    float2  Garb;
};

float4 main(PostProcessVertexToPixel input) : SV_Target0
{
    float2 uv = input.myUV;
    
    float3 entD = GBufferEntData.Sample(GBufferSampler, uv).rgb;
    if(entD.r == 0)
        discard;
    float3 worldPos = GBufferPosition.Sample(GBufferSampler, uv).rgb;
       
    // Use the world position, and transform by the previous view-    
    // projection matrix.    
    float4 previousPos = mul(myPrevWorldToClipMatrix, float4(worldPos.xyz, 1));
    float2 currentPos  = uv.xy;
    // Convert to nonhomogeneous points [-1,1] by dividing by w. 
    previousPos /= previousPos.w;  
    previousPos.y *= -1;
    previousPos = previousPos * 0.5 + 0.5; 
    // Use this frame's position and last frame's to compute the pixel    
    // velocity.    
    float3 color = 0;
    float2 velocity = (currentPos.xy - previousPos.xy)/2.f; 
    float2 factor = (velocity.xy * myWeight) / (exp(1 / myWeight));
    int i = 0; 
    for (; i < mySamples; i++)
    {
        color.rgb += FullscreenTexture.Sample(GBufferSampler, uv).rgb;
        uv.xy -= factor.xy * pow(i + 2, 2); //weight component that gets scaled with numSamples
    } 
    //i = 0;
    //float3 tColor = 0;
    //uv = input.myUV;
    //velocity = entD.gb;
    //velocity = velocity * 2 - 1;
    //factor = (velocity * myWeight) / (exp(1 / myWeight));
    ////float weightInv = (1.f / myWeight);
    //if (length(velocity) != 0)
    //{   
    //    for (; i < mySamples; i++)
    //    {
    //        tColor += FullscreenTexture.Sample(GBufferSampler, uv);
    //        uv -= factor * pow(i + 2, 2); //weight component that gets scaled with numSamples
    //    }
    //}
    return float4((color.xyz) / (mySamples), 1);
}