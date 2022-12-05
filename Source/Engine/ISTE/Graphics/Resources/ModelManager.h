#pragma once
#include <string>
#include <vector>
#include <wrl/client.h> 
#include <unordered_map>

#include "ISTE/CU/HashMap.hpp" 
#include "ISTE/Math/Vec.h" 
#include "ISTE/ECSB/ECSDefines.hpp"
#include "ISTE/Graphics/RenderDefines.h"
#include "ISTE/Graphics/RenderTarget.h" 
#include "ISTE/Graphics/Camera.h" 

#include "ISTE/Graphics/Resources/Model.h"
#include "ISTE/Graphics/Resources/Animation.h" 
#include "ISTE/Graphics/Resources/FBXLoader.h"

using Microsoft::WRL::ComPtr;
struct ID3D11InputLayout;
struct ID3D11PixelShader;
struct ID3D11VertexShader;
struct ID3D11Buffer; 

namespace ISTE
{
	struct Context;
	class SceneHandler;

	class ModelManager
	{
	public: //structs
		struct ModelLoadResult {
			operator ModelID() { return myValue; }
			operator bool() { return mySuccesFlag; }

			bool mySuccesFlag;
			ModelID myValue;
		};
		struct AnimationLoadResult {
			operator AnimationID() { return myValue; }
			operator bool() { return mySuccesFlag; }

			bool mySuccesFlag;
			AnimationID myValue;
		};

	public:
		ModelManager();
		~ModelManager();
		bool Init();

#pragma region "Loading models"

		/*
		* Adding the model to the ModelComponent is done manually 
		* @param aModelPath: The file path to the model
		*/
		ModelLoadResult LoadModel(std::string aModelPath);

		/*
		* Will handle component management automatically.
		* @param aEntityID: The entity to add stuff to
		* @param aModelPath: The file path to the model
		*/
		ModelLoadResult LoadModel(EntityID aEntityID, std::string aModelPath); 

		/*
		 @param aSubDiv: The subdivision count of the created plane
		*/
		int LoadPlane(int aSubDiv);

		/*
		 Loads a cube primitive
		*/
		int LoadUnitCube();

		/*
		 Loads a sprite quad
		*/
		int LoadSpriteQuad(); 
#pragma endregion

#pragma region "cpu-gpu"

		/*
		* Binds a mesh to the gpu
		* @param aModelId: The id to a model
		* @param aMeshIndex: The mesh to bind to the gpu 
		*/
		void BindMesh(ModelID aModelId, int aMeshIndex);

		/*
		* Renders a mesh
		* @param aModelId: The id to a model
		* @param aMeshIndex: The mesh to render
		*/
		void RenderMesh(ModelID aModelId, int aMeshIndex);

		/*
		* Renders a Model
		* @param aModelId: The id to the model to render
		*/
		void RenderModel(ModelID aModelId);

#pragma endregion

#pragma region "misc" 
		
		/* 
		* overwrites all meshes with a texture 
		* @param aModelId: The id to the model to render
		* @param aTextureIt: The iterator to the aTexture
		* @param aTextureType: The type of texture ie Albedo, Normal, Material maps.
		* 
		* Defines for the texture type are in RenderDefines.h
		*/
		void OverwriteTexture(ModelID aModelId, TextureID aTextureIt, int aTextureType);

		//@return the mesh count
		int GetMeshCount(ModelID aModelId);

		//@return the ModelType
		ISTE::ModelType GetModelType(ModelID aModelId);

		//
		std::unordered_map<std::string, int>& GetBoneNameToIdMap(ModelID aModelId);

		Model* GetModel(ModelID aModelIndex) { return myModels.GetByIterator(aModelIndex); }
		//Animation* GetAnimation(AnimationID aAnimIndex) { return myAnimations.GetByIterator(aAnimIndex); }
		RenderTarget& GetPreviewRenderTarget(ModelID aModelIndex) { return myModelPreviews[aModelIndex]; }


		CU::HashMap<std::string, Model*, MAX_CHACHED_MODEL_COUNT, ModelID>& GetModelList() { return myModels; }
		//CU::HashMap<std::string, Animation*, MAX_CACHED_ANIMATIONS_COUNT, AnimationID>& GetAnimationList() { return myAnimations; }
#pragma endregion 

	private: //Data and stuff
		Context* myCtx;
		SceneHandler* mySceneHandler; 
		FBXLoader myFbxLoader; 

		CU::HashMap<std::string, Model*, MAX_CHACHED_MODEL_COUNT, ModelID> myModels;
		CU::HashMap<std::string, Animation*, MAX_CACHED_ANIMATIONS_COUNT, AnimationID> myAnimations;
		RenderTarget myModelPreviews[MAX_CHACHED_MODEL_COUNT];

	private: //Assimp processing
		ModelLoadResult LoadNewModel(std::string aModelPath); 
		//void GetNodeNames(std::vector<std::string>& aOutList, aiNode* aNode);  
		//CU::Matrix4x4f GetTransformOfBone(int aModelIt, aiNode*); 
	
	private: //Shader Stuff
		Camera myPreviewCamera;

		bool InitShaders(const char* aVsPath, const char* aPsPath);
		void BindShaders();
		void BindObjBuffer(CU::Matrix4x4f aTransform); 

		ComPtr<ID3D11PixelShader>	myPreviewPS;
		ComPtr<ID3D11VertexShader>	myPreviewVS;
		ComPtr<ID3D11InputLayout>	myPreviewLayout;
		ComPtr<ID3D11Buffer>		myPreviewObjBuffer;
	};

};