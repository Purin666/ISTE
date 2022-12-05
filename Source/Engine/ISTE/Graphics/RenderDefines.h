#pragma once 

#define MAX_TEXTURE_COUNT 1024
#define MAX_MATERIAL_COUNT 3 
#define ALBEDO_MAP 0
#define NORMAL_MAP 1
#define MATERIAL_MAP 2

#define MAX_CHACHED_MODEL_COUNT 1024
#define MAX_MESH_COUNT 32

#define MAX_CACHED_ANIMATIONS_COUNT 256
#define MAX_ANIMATION_COUNT 32
#define MAX_KEYFRAME_COUNT 128
#define MAX_BONE_COUNT 65
#define MAX_BONE_CHILDREN 4
#define MAX_BONE_WEIGHT_COUNT 4

#define LINE_DRAWER_INSTANCE_BATCH_SIZE 2048
#define MODEL_INSTANCE_BATCH_SIZE 1024 
#define MODEL_X_LIGHT_BATCH_SIZE 100 
#define RENDER_COMMAND_SIZE 2048

#define MAX_SSAO_KERNAL_SIZE 64

/*
	render flags
	the first 8 is for identification of world elements for the rendering.
		-	for example, we can check if we are drawing a decal on a player when we just want to draw it on the geometry
*/
enum class RenderFlags {
	Environment	= 1 << 0, 
	Player		= 1 << 1,
	Enemies		= 1 << 2, 
};

inline RenderFlags operator|(const RenderFlags& aLeft, const RenderFlags& aRight)
{
	return (RenderFlags)((int)aLeft | (int)aRight);
}

typedef unsigned short AnimationBlendID;
typedef unsigned short AnimationID;
typedef unsigned short TextureID;
typedef unsigned short ModelID;
typedef unsigned short FontID;

//#define MaxPointLightCount 8
//#define MaxSpotLightCount 2


//enums

//should do something about theese
namespace ISTE {

	enum class ModelType {
		eStaticModel,
		eAnimatedModel,
		eTransparantModel,

		eCount,
	}; 
};