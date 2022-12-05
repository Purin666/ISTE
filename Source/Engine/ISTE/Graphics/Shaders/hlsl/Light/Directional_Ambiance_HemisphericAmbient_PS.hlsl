#include "../CommonPP.hlsli"
#include "../Common.hlsli"
#include "../PBRFunctions.hlsli"
 

float4 main(PostProcessVertexToPixel anInput) : SV_TARGET
{ 
    float2 uv = anInput.myUV;
    
    float4 worldPos = GBufferPosition.Sample(DefaultSampler,       uv).rgba;
    float4 albedo   = GBufferAlbedo.Sample(DefaultSampler,         uv).rgba; 
    float4 pNormal  = GBufferPixelNormal.Sample(DefaultSampler,    uv).rgba;
    float4 vNormal  = GBufferPixelNormal.Sample(DefaultSampler,    uv).rgba;
    float4 material = GBufferMaterial.Sample(DefaultSampler,       uv).rgba;
    float4 cData    = GBufferCustomData.Sample(DefaultSampler,     uv).rgba;
    
    
    pNormal.xyz = normalize((pNormal.xyz * 2.f) - 1.f);
    vNormal.xyz = normalize((vNormal.xyz * 2.f) - 1.f);
     
    //lista up pbr material grejer som variabler
    //eller ha som kommentar 
    float metallic  = material.r;
    float roughness = material.g;
    float emissive  = material.b;
    float emisStr   = material.a;
    
    
    float3 toEye = normalize(myCamPosition - worldPos.xyz);
    float3 specularColor = lerp((float3) 0.04f, albedo.rgb, metallic);
    float3 diffuseColor = lerp((float3) 0.00f, albedo.rgb, 1 - metallic);
    //cData.x = 1;
    
    
    float3 directionalLight = EvaluateDirectionalLight(diffuseColor, specularColor, pNormal.xyz, roughness,
        (myDirectionalLightColor.rgb * myDirectionalLightColor.a), -myDirectionalLightDir, toEye.xyz
    ); 
    
    
    float ssao = SSAOTexture.Sample(GBufferSampler, uv).r; 
    float3 toEyeRotated = mul(myAmbLightRotMatrix, float4(toEye.xyz, 0)).xyz;
    float3 ambiance = EvaluateAmbiance(CubeMap, pNormal.xyz, vNormal.xyz, toEyeRotated.xyz,
        roughness, cData.x, diffuseColor, specularColor
    );
    
    //two way ambiance
    float3 hemisphericLight = ((0.5f + 0.5f * pNormal.y) * (mySkyColor.rgb * mySkyColor.a))
                              + ((0.5f - 0.5f * pNormal.y) * (myGroundColor.rgb * myGroundColor.a));
     
    //dirlight shadow
    float shadowFactor = 1.f;
    float4 ProjectedPositionTemp = mul(myLightSpaceMatrix, worldPos);
    float3 ProjectedPosition = ProjectedPositionTemp.xyz / ProjectedPositionTemp.w;
    float computedZ = ProjectedPosition.z;
    
    float2 texelSize = 1.f / 2000.f;
    float n = 2;
    float bias = 0.0001;
    float texelOffsetScale = 1;
    
    if (clamp(ProjectedPosition.x, -1.0, 1.0) == ProjectedPosition.x &&
        clamp(ProjectedPosition.y, -1.0, 1.0) == ProjectedPosition.y)
    {
        //no clamp because it can be achieved by the hardware through addres mode clamp
        for (float i = -n; i < n; i++)
            for (float j = -n; j < n; j++)
            {
                float2 offset = float2(i, j) * texelOffsetScale * texelSize;
                float2 uvPos = 0.5f + float2(0.5f, -0.5f) * ProjectedPosition.xy;
                uvPos += offset;

                float shadowMapZ = ShadowMap.Sample(ShadowSampler, uvPos).x;
                shadowFactor += (computedZ < shadowMapZ);
            }
        shadowFactor /= pow(n * 2, 2);
    }
    
    float3 emissiveAlbedo = albedo.rgb * emissive * (emisStr * 100);
    float3 ambientTerm = (((ambiance * myCubeMapIntensity) + hemisphericLight) * ssao);
    float3 dirLight = directionalLight * shadowFactor;
    return float4(dirLight + emissiveAlbedo + ambientTerm, 1);
}