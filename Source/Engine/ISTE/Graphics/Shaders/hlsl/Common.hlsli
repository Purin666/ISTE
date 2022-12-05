//defines
#define MaxBoneCount 65
#define ModelBatchSize 1024
#define MaxSSAOKernalSize 64
//#define MaxPointLights 8
//#define MaxSpotLights 2

//textures
Texture2D AlbedoTex             : register(t0);       //or albedo texture
Texture2D NormalTex             : register(t1);       //or normal texture
Texture2D MaterialTex           : register(t2);       //or material texture 

TextureCube CubeMap             : register(t3); 
 
Texture2D GBufferPosition       : register(t4);
Texture2D GBufferAlbedo         : register(t5);
Texture2D GBufferPixelNormal    : register(t6); 
Texture2D GBufferVertexNormal   : register(t7);
Texture2D GBufferMaterial       : register(t8);
Texture2D GBufferCustomData     : register(t9); 
Texture2D GBufferEntData        : register(t10);

Texture2D       SSAOTexture        : register(t11); 
Texture2D       ShadowMap          : register(t12);
TextureCube     ShadowCubes        : register(t13);

//sampler
SamplerState    DefaultSampler     : register(s0);
SamplerState    CubeMapSampler     : register(s1);
SamplerState    GBufferSampler     : register(s2);
SamplerState    ShadowSampler      : register(s3);

//buffers
cbuffer LightBuffer : register(b0)
{
    float4x4    myLightSpaceMatrix;
    float4x4    myAmbLightRotMatrix;
    
    float3      myDirectionalLightDir; 
    float       myCubeMapIntensity;
    float4      myDirectionalLightColor; 
    float4      mySkyColor;
    float4      myGroundColor; 
};

cbuffer FrameBuffer : register(b1)
{
    float4x4    myWorldToClipMatrix;   
    float4x4    myPrevWorldToClipMatrix;   
    float4x4    myWorldToView;   
    float4x4    my2DWorldToClipMatrix;
    float3      myCamPosition; float unused1;
    float2      myResolution;                          //x width, y height
    float2      myTimings;                             //x delta, y total time
};

//buffer slot 2 is reserved for obj buffers 
//maybe its a good idea to move them here or to a nother hlsli file

cbuffer PostProcessBuffer : register(b8)
{
    float3  myContrast;
    float   myBloomBlending;
    float3  myTint;
    float   mySaturation;
    float3  myBlackpoint;
    float   myExposure;
}

//structs
struct VertexInputType
{
    float3 myPosition           : POSITION;
    float4 myColor              : COLOR; 
    float2 myUvCoordinates      : UV; 
    float3 myNormal             : NORMAL;
    float3 myBiNormal           : BINORMAL;
    float3 myTangent            : TANGENT;
    float4 myBones              : BONES;              //4 bones will be able to effect a vertex
    float4 myWeights            : WEIGHTS;
    uint   myVertexIndex        : SV_VertexID;
};

struct InstancedVertexInput
{
    float3 myPosition           : POSITION;
    float4 myColor              : COLOR;
    float2 myUvCoordinates      : UV;
    float3 myNormal             : NORMAL;
    float3 myBiNormal           : BINORMAL;
    float3 myTangent            : TANGENT;
    float4 myBones              : BONES; //4 bones will be able to effect a vertex
    float4 myWeights            : WEIGHTS;
    float4x4 myWorldTransform   : InstanceWorld; 
    float4 myScaleAndOffset     : InstancedUVScaleAndOffset;
    uint myInstanceID           : SV_InstanceID;
    uint myVertexID             : SV_VertexID;
};


struct VFXPixelInputType
{
    float4 myPosition : SV_POSITION;
    float3 myWorldPosition : POSITION;
    float4 myColor : COLOR;
    float2 myUvCoordinates : UV;
    float3 myNormal : NORMAL;
    float3 myBiNormal : BINORMAL;
    float3 myTangent : TANGENT;
};

struct PixelInputType
{
    float4 myPosition       : SV_POSITION;
    float3 myWorldPosition  : POSITION0;
    float4 myColor          : COLOR;
    float2 myUvCoordinates  : UV;
    float3 myNormal         : NORMAL;
    float4 myPrevPos        : POSITION1;
    float3 myBiNormal       : BINORMAL;
    float3 myTangent        : TANGENT; 
};

struct InstancedPixelInputType
{
    float4 myPosition : SV_POSITION;
    float3 myWorldPosition : POSITION;
    float4 myColor : COLOR;
    float2 myUvCoordinates : UV;
    float3 myNormal : NORMAL;
    float3 myBiNormal : BINORMAL;
    float3 myTangent : TANGENT;
    uint2  myIDs    : IDs;           //r = vertexId,  g = instanceId 
};


struct ColorAndEntityOutput
{
    float4  myAlbedo    : SV_Target0; //r = r, g = g, b = b, a = 0
    float1   myEntities : SV_Target1;
};

struct GBufferOutput
{
    float4 myWorldPosition  : SV_Target0;           //could just be reconstructed trough depth and stored somewhere else
    float4 myAlbedo         : SV_Target1;           //r = r, g = g, b = b, a = 0
    float4 myPixelNormal    : SV_Target2;           //r = pnX, g = pnY, b = pnZ, a = 0 
    float4 myVertexNormal   : SV_Target3;           //r = vnX, g = vnY, b = vnZ, a = 0 
    float4 myMaterial       : SV_Target4;           //r = metallnes, g = roughness, b = emissive, a = eStr
    float4 myCustomData     : SV_Target5;           //r = ambiend occlusion, g = depth, b = RenderFlags, a = 0  
    float4 myEntitynVelData : SV_Target6;           //r = Entity, g = Vel.x, b = Vel.y  
};

//Statics
static uint2 textureRectLookup[6] =
{
    uint2(0, 3),
	uint2(0, 1),
	uint2(2, 1),
	uint2(2, 3),
	uint2(2, 1),
	uint2(0, 3),
};

//FUNCS 
float4x4 FaceTowards(float3 aToPos, float4x4 aMatrix)
{
    float4x4 outMat = 0;
    float3 scale = float3(
        length(aMatrix[0].xyz),
        length(aMatrix[1].xyz),
        length(aMatrix[2].xyz)
        );
    float3 fromPos = float3(
        aMatrix[0].w, 
        aMatrix[1].w,  
        aMatrix[2].w
        );
    
    float3 nDiff = normalize(aToPos - fromPos);
    float3 right = normalize(cross(nDiff, float3(0, 1, 0))); //or cam upp dir
    float3 up = normalize(cross(right, nDiff));
    
    outMat[0].rgb  = right.rgb * scale.x;
    outMat[1].rgb  = up.rgb * scale.y;
    outMat[2].rgb  = nDiff.rgb * scale.z;
    outMat[3].rgba = float4(fromPos.xyz, 1);
    return outMat;
}

float4x4 FaceTowards(float3 aToPos, float3 aFromPos, float3 aScale)
{
    float4x4 outMat;  
    
    float3 nDiff = normalize(aToPos - aFromPos);
    float3 right = normalize(cross(nDiff, float3(0, 1, 0))); //or cam upp dir
    float3 up = normalize(cross(right, nDiff));
    
    outMat[0].rgb = right.rgb * aScale.x;
    outMat[1].rgb = up.rgb * aScale.y;
    outMat[2].rgb = nDiff.rgb * aScale.z;
    outMat[3].rgba = float4(aFromPos.xyz, 1);
    return outMat;
}

float3 expandNormal(float4 normalTexture)
{
    float3 normal = normalTexture.agg;
    normal = 2.0f * normal - 1.0f;
    normal.z = sqrt(1 - saturate(normal.x * normal.x + normal.y * normal.y));
    return normalize(normal);
}

float2 GetUVRect(float4 aRect, uint aIndex)
{
    uint2 theLookuped = textureRectLookup[aIndex];
    return float2(aRect[theLookuped.x], aRect[theLookuped.y]);
}

float3x3 invertMatrix(float3x3 m)
{
	// computes the inverse of a matrix m
    float det = m[0][0] * (m[1][1] * m[2][2] - m[2][1] * m[1][2]) -
		m[0][1] * (m[1][0] * m[2][2] - m[1][2] * m[2][0]) +
		m[0][2] * (m[1][0] * m[2][1] - m[1][1] * m[2][0]);

    float invdet = 1 / det;

    float3x3 minv; // inverse of matrix m
    minv[0][0] = (m[1][1] * m[2][2] - m[2][1] * m[1][2]) * invdet;
    minv[0][1] = (m[0][2] * m[2][1] - m[0][1] * m[2][2]) * invdet;
    minv[0][2] = (m[0][1] * m[1][2] - m[0][2] * m[1][1]) * invdet;
    minv[1][0] = (m[1][2] * m[2][0] - m[1][0] * m[2][2]) * invdet;
    minv[1][1] = (m[0][0] * m[2][2] - m[0][2] * m[2][0]) * invdet;
    minv[1][2] = (m[1][0] * m[0][2] - m[0][0] * m[1][2]) * invdet;
    minv[2][0] = (m[1][0] * m[2][1] - m[2][0] * m[1][1]) * invdet;
    minv[2][1] = (m[2][0] * m[0][1] - m[0][0] * m[2][1]) * invdet;
    minv[2][2] = (m[0][0] * m[1][1] - m[1][0] * m[0][1]) * invdet;

    return minv;
}