#include "../Common.hlsli"
#include "../CommonPP.hlsli"

GBufferOutput main(PostProcessVertexToPixel anInput)
{
    float2 uv = anInput.myUV;
    
    GBufferOutput output;
    output.myWorldPosition      = GBufferPosition.Sample(DefaultSampler, uv).rgba;
    output.myAlbedo             = GBufferAlbedo.Sample(DefaultSampler, uv).rgba;
    output.myPixelNormal.xyz    = GBufferPixelNormal.Sample(DefaultSampler, uv).rgba;
    output.myVertexNormal       = GBufferVertexNormal.Sample(DefaultSampler, uv).rgba;
    output.myMaterial           = GBufferMaterial.Sample(DefaultSampler, uv).rgba;
    output.myCustomData         = GBufferCustomData.Sample(DefaultSampler, uv).rgba; 
    output.myEntitynVelData     = GBufferEntData.Sample(DefaultSampler, uv).rgba;
    return output;
}