#include "../Common.hlsli"

cbuffer CommonObjbuffer : register(b2)
{
    struct InstanceData
    {
        float3  myColor;
        uint    myObjId;
        float   myRenderFlags;
        float3 myGRAHB;
    } myInstanceData[ModelBatchSize];
};

GBufferOutput main(InstancedPixelInputType aInput)
{
 
    //GATHERS DATA FROM ALBEDO
    float4 albedo = AlbedoTex.Sample(DefaultSampler, aInput.myUvCoordinates.xy).rgba;
    if (albedo.a < 0.2)
        discard;
    
    //GATHERS DATA FROM NORMAL MAP  
    float4 sampledNormalMap = NormalTex.Sample(DefaultSampler, aInput.myUvCoordinates.xy).rgba;
    float3 normal = expandNormal(sampledNormalMap);
    float ao = sampledNormalMap.b;
    
    float3x3 tbnMatrix = float3x3(
        aInput.myTangent,
        aInput.myBiNormal,
        aInput.myNormal
    );
    float3 pixelNormal = normalize(mul(normal, tbnMatrix));

    //GATHERS DATA FROM MATERIALMAP
    float4 material = MaterialTex.Sample(DefaultSampler, aInput.myUvCoordinates.xy);
   
    float3 color = albedo.rgb * myInstanceData[aInput.myIDs.g].myColor; 
    
    GBufferOutput output;
    output.myWorldPosition      = float4(aInput.myWorldPosition.rgb, 1);
    output.myAlbedo             = float4(color, 1);
    output.myPixelNormal.xyz    = float4(0.5f + pixelNormal.rgb * 0.5f, 1);
    output.myVertexNormal       = float4(0.5f + aInput.myNormal.rgb * 0.5f, 1);
    output.myMaterial           = material; 
    output.myCustomData         = float4(ao, aInput.myPosition.z, myInstanceData[aInput.myIDs.g].myRenderFlags, 1);
    output.myEntitynVelData     = float4(myInstanceData[aInput.myIDs.g].myObjId + 1,0.5,0.5,1);
	
    return output;
}