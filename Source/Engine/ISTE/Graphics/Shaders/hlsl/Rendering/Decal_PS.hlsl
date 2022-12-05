#include "../Common.hlsli"

cbuffer ObjBuffer : register(b2)
{ 
    float4x4 myModelToWorldInv;
    float4x4 myModelToWorld;
    
    float1 myCoverageFlags;
    float3 myLerpValues;
    
    float1 myEntityIndex;
    float1 myThreshold;
    float2 myGarb;
}

GBufferOutput main(PixelInputType anInput)
{
    GBufferOutput output;
    float2 uv = anInput.myPosition.xy / myResolution.xy;
    
    float4 worldPos = GBufferPosition.Sample(DefaultSampler, uv).rgba;
    float4 albedo   = GBufferAlbedo.Sample(DefaultSampler, uv).rgba;
    float4 pNormal  = GBufferPixelNormal.Sample(DefaultSampler, uv).rgba;
    float4 vNormal  = GBufferVertexNormal.Sample(DefaultSampler, uv).rgba;
    float4 material = GBufferMaterial.Sample(DefaultSampler, uv).rgba;
    float4 cData    = GBufferCustomData.Sample(DefaultSampler, uv).rgba;
    float4 eData    = GBufferEntData.Sample(DefaultSampler, uv).rgba;
    
    float1 renderFlag = cData.b;
    
    pNormal.xyz = normalize((pNormal.xyz * 2.f) - 1.f);
    vNormal.xyz = normalize((vNormal.xyz * 2.f) - 1.f);
     
    
    //Convert from world space to object space
    float4 boxSpacePos = mul(myModelToWorldInv, float4(worldPos.xyz, 1));
    //Perform bounds check
    clip(0.5 - abs(boxSpacePos.xyz));
    float2 boxUV = boxSpacePos.xz + 0.5;
    boxUV.x *= -1.f;
    
    float3 ass = {
        myModelToWorld[0].y,
        myModelToWorld[1].y,
        myModelToWorld[2].y
    };
    clip(-myThreshold + dot(pNormal.xyz, ass));
    
    
    float4 decalColor       = AlbedoTex.Sample(DefaultSampler, boxUV).rgba;
    float3 decalNormal      = expandNormal(NormalTex.Sample(DefaultSampler, boxUV).rgba);
    float4 decalMaterial    = MaterialTex.Sample(DefaultSampler, boxUV).rgba;
    
    if (!((int) renderFlag & (int) myCoverageFlags))
        discard;
    
    //albedo
    albedo.xyz = lerp(decalColor.xyz, albedo.xyz, myLerpValues[0]);  
    
    // get edge vectors of the pixel triangle 
    float3 p    = myCamPosition - worldPos.xyz;    
    float3 dp1  = ddx(p);
    float3 dp2  = ddy(p); 
    float2 duv1 = ddx(boxUV);
    float2 duv2 = ddy(boxUV);
    
    float3 N = normalize(vNormal);
    // solve the lin­ear sys­tem
    float3 dp2perp = cross(N, dp2);
    float3 dp1perp = cross(dp1, N);
    float3 T = dp2perp * duv1.x + dp1perp * duv2.x;
    float3 B = dp2perp * duv1.y + dp1perp * duv2.y;
    float1 invmax = sqrt(pow(max(dot(T, T), dot(B, B)),-1));
    float3x3 TBN = { 
        T * invmax,
        B * invmax,
        N 
        };
    
    //Transform normal from tangent space into view space
    float3 pixelNormal = mul(TBN, decalNormal); 
    //normal
    pixelNormal.x *= -1;
    pNormal.xyz = lerp(pixelNormal.xyz, pNormal.xyz, myLerpValues[1]);
    //material
    material.xyz = lerp(decalMaterial.xyz, material.xyz, myLerpValues[2]);  
    
    output.myWorldPosition      = float4(worldPos.xyz               , 1);
    output.myAlbedo             = float4(albedo.xyz                 , decalColor.w);
    output.myPixelNormal        = float4(0.5f + pNormal.xyz * 0.5f  , decalColor.w);
    output.myVertexNormal       = float4(0.5f + vNormal.xyz * 0.5f  , 1);
    output.myMaterial           = float4(material.xyz               , decalColor.w);
    output.myCustomData         = float4(cData.xyz                  , 1);
    output.myEntitynVelData     = float4(myEntityIndex + 1, eData.g, eData.b, 1);
    return output;
}