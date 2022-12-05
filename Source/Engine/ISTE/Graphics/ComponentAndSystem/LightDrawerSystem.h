#pragma once
#include <vector>
#include <set>
#include <wrl/client.h> 

#include "ISTE/Graphics/DepthStencilTarget.h"
#include "ISTE/Graphics/CubeDepthStencilTarget.h"
#include "ISTE/ECSB/System.h"
#include "ISTE/Math/Vec.h"
#include "ISTE/Math/Matrix4x4.h"
#include "ISTE/Graphics/Camera.h"
#include "ISTE/Graphics/RenderDefines.h"


using Microsoft::WRL::ComPtr;
struct ID3D11InputLayout;
struct ID3D11PixelShader;
struct ID3D11VertexShader;
struct ID3D11Buffer;

namespace ISTE
{
	struct Context;
	class LightDrawerSystem : public System
	{
	public:
		enum class LightComplexity{
			eDirectionalLight,
			eAmbientLight,
			ePointLight,
			eShadowCastingPointLight,
			eSpotLight,
			eShadowCastingSpotLight,
			eCount,
		};

		struct  PLC {
			Camera					myCamera;
			CU::Matrix4x4f			myModelTransforms;
			CU::Vec4f				myColorAndIntensity;
			EntityIndex				myEntityIndex;
			std::set<EntityID>		myEnts;
			float					myRange = 0.f;
		};

		struct SLC {
			Camera					myCamera;
			CU::Matrix4x4f			myModelTransforms;
			CU::Vec4f				myColorAndIntensity;
			CU::Vec4f				myDirection;
			EntityIndex				myEntityIndex;
			std::set<EntityID>		myEnts;

			float	myInnerLimit = 0.f;
			float	myOuterLimit = 0.f;
			float	myRange = 0.f;
		};

		//currently this is used for icon generation.
		//later i want to change that dir and amb light rendering actually uses render commands
		struct DLC {
			CU::Matrix4x4f	myModelToWorldTransforms;
			EntityIndex		myEntityIndex;
		};

		struct ALC {
			CU::Matrix4x4f	myModelToWorldTransforms;
			EntityIndex		myEntityIndex;
		};

	public:
		bool Init();
		void PrepareLightCommands();
		void PrepareLightIcons();
		void RenderLights();
		void ClearRenderCommands();

		void SetDirectionalLight(EntityID aID);

		inline void SetAmbientLight(EntityID aID) { myActiveAmbientLight = aID; }
		inline const EntityID GetDirectionalLight() { return myActiveDirectionalLight; }
		inline const EntityID GetAmbientLight() { return myActiveAmbientLight; } 
	private:
		void RecalculateLight(EntityID aId, float aRadius, CU::Vec3f aPos, std::set<EntityID>& aEntList);
		void UpdateShadowCastersEntityList(EntityID aId);
		void RemoveFromShadowCasterEntityList(EntityID aId);

		void PrepareSpotLights();
		void PreparePointLights();
		void PrepareShadowSpotLights();
		void PrepareShadowPointLights();
		void PrepareDirectionalLights();
		void PrepareAmbientLights();
		
		void DrawPointLights();
		void DrawSpotLights();
		void DrawShadowCastingPointLights();
		void DrawShadowCastingSpotLights();
		void DrawDirectionalAndAmbientLight();

		void InitPointLightMatrixList();


		Context* myCtx;
		std::vector<PLC> myPointLightCommands;
		std::vector<SLC> mySpotLightCommands;
		std::vector<PLC> myShadowPointLightCommands;
		std::vector<SLC> myShadowSpotLightCommands;

		std::vector<DLC> myDirectionalLightCommands;
		std::vector<ALC> myAmbientLightCommands;

		CubeDepthStencilTarget myLightCubeDepthTarget;
		DepthStencilTarget myLightDepthTarget;
		EntityID myActiveDirectionalLight = EntityIndex(-1);
		EntityID myActiveAmbientLight = EntityIndex(-1);
		 
		CU::Matrix4x4f	myPLTransforms[6];

		ModelID		myPLightModel;
		TextureID	myLightIcons[(int)LightComplexity::eCount];


	private: //Shader Stuff
		void BindPointLightBuffer(PLC& aCommand);
		void BindSpotLightBuffer(const SLC& aCommand);
		void BindShader(LightComplexity);

		bool InitShader(std::string aVSPath, std::string aPSPath, LightComplexity);
		bool InitPointLightBuffer();
		bool InitSpotLightBuffer(); 

		ComPtr<ID3D11Buffer>		myObjBuffer[(int)LightComplexity::eCount];
		ComPtr<ID3D11InputLayout>	myInputLayout[(int)LightComplexity::eCount];
		ComPtr<ID3D11VertexShader>	myVertexShader[(int)LightComplexity::eCount];
		ComPtr<ID3D11PixelShader>	myPixelShader[(int)LightComplexity::eCount];


	};
};