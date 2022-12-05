#pragma once
#include <wrl/client.h> 
#include <vector>
#include <set>

#include "ISTE/Graphics/GBuffer.h"
#include "ISTE/Graphics/Shaders/cpp/FullscreenEffect.h"
#include "ISTE/Graphics/DebugDrawer.h"
#include "ISTE/Graphics/PP_FS_Effects/SSAOEffect.h"
#include "ISTE/Graphics/PP_FS_Effects/MotionBlurEffect.h"
#include "ISTE/Graphics/PP_FS_Effects/ChromaticAberrationEffect.h"
#include "ISTE/Graphics/PP_FS_Effects/RadialBlurEffect.h"
#include "ISTE/ECSB/ECSDefines.hpp"
#include "Camera.h" 


using Microsoft::WRL::ComPtr;

struct ID3D11Buffer;  
namespace ISTE {
	class SpriteDrawerSystem;
	class VFXSpriteDrawerSystem;
	class ModelDrawerSystem;
	class CustomDrawerSystem;
	class TransperancyDrawerSystem;
	class TransformSystem;
	class AnimationDrawerSystem;
	class CameraSystem;
	class LightDrawerSystem;
	class TextSystem;
	class DecalDrawerSystem;

	struct Context;

	struct LightBuffer {
		CU::Matrix4x4f  myLightSpaceMatrix;
		CU::Matrix4x4f  myAmbLightRotMatrix;
		CU::Vec3f       myDLdir;                      
		float			myCubeMapIntensity  = 1.f;
		CU::Vec4f       myDLColorAndIntensity;
		CU::Vec4f       myALGroundColorAndIntensity;
		CU::Vec4f       myALSkyColorAndIntensity;
	};

	struct FrameBuffer {
		CU::Matrix4x4f  myWorldToClip;
		CU::Matrix4x4f  myPrevWorldToClip;
		CU::Matrix4x4f  myWorldToView;
		CU::Matrix4x4f  my2DWorldToView;
		CU::Vec3f       myCamPos;           float a;
		CU::Vec2f       myResolution;       //x width, y height
		CU::Vec2f       myTimings;          //x Delta, y Total
		//(maybe look direction?)
	};

	struct FogBuffer {
		CU::Vec3f		myFogColor			= {1.0f,0.1f,0.9f};
		float			myFogStartDist		= 10.f;

		CU::Vec3f		myFogHighlightColor	= {0.f,0.f,0.f};
		float			myFogGlobalDensity	= 0.0f;
		 
		float			myFogHeightFalloff	= 0.1f;
		CU::Vec3f		myGarb;
	};

	struct PostProcessBuffer {      // default values
		CU::Vec3f		myContrast		= { 1.f, 1.f, 1.f };
		float			myBloomBlending = 1.f;
		CU::Vec3f		myTint			= { 1.f, 1.f, 1.f, };
		float			mySaturation	= 1.f;
		CU::Vec3f		myBlackpoint	= { 0.f, 0.f, 0.f };
		float			myExposure		= 0.f;
	}; 

	struct GPUOutData { 
		float myAo;
		float myId;
		float myDepth;
		float junk2;
	};

	class GraphicsEngine
	{
	public:
		GraphicsEngine();
		~GraphicsEngine();

		bool Init(); 
		void PrepareRenderCommands();
		void Render(); 
		void RenderDepth();
		void RenderDepth(std::set<EntityID>&);
		void ClearCommands();
		void CopyToBackBuffer();

		GPUOutData Pick(const CU::Vec2Ui& aMouseCoord);
		
		void BindInitBuffer();
		void BindFrameBuffer();
		void BindLightBuffer();
		void BindLightBuffer(const CU::Matrix4x4f& aLightSpaceMatrix, const CU::Vec3f& aDlDir);
		void BindPostProcessBuffer();
		void BindFogBuffer();
		void DownAndUpSample();

		void SelectRenderPass();

		inline DebugDrawer& GetDebugDrawer() { return myDebugDrawer; }
		inline void SetCamera(Camera& aCamera) { myCamera = &aCamera; }
		inline Camera& GetCamera() { return *myCamera; }

		LightBuffer& GetLightBuffer()					{ return myLightBufferData; }
		FrameBuffer& GetFrameBuffer()					{ return myFrameBufferData; }
		PostProcessBuffer& GetPostProcessBuffer()		{ return myPPdata; }
		FogBuffer& GetFogBuffer()						{ return myFogBufferData; }
		SSAOEffect& GetSSAOEffect()						{ return mySSAOEffect; }
		MotionBlurEffect& GetMBlurEffect()				{ return myMotionBlurEffect; }
		ChromaticAberrationEffect& GetAberrationEffect(){ return myChromaticAberrationEffect; }
		RadialBlurEffect& GetRadialBlur()				{ return myRadialBlurEffect; }
		
	private: //bufferData
		LightBuffer			myLightBufferData;
		FrameBuffer			myFrameBufferData;
		PostProcessBuffer	myPPdata;
		FogBuffer			myFogBufferData; 
	private:
		friend class DX11;
		Camera* myCamera = nullptr;

		void DrawToGBuffer();
		void DrawLight(); 
		void PostProcess();

		bool CreateBuffers(); 

		Context* myCtx;
		ComPtr<ID3D11Buffer>		myFrameBuffer;
		ComPtr<ID3D11Buffer>		myLightBuffer;
		ComPtr<ID3D11Buffer>		myPostProcessBuffer;
		ComPtr<ID3D11Buffer>		myFogBuffer; 

		DebugDrawer					myDebugDrawer;
		CU::Matrix4x4f				my2DWorldToClip;
		CU::Matrix4x4f				myPrevWTCMatrix;
		SSAOEffect					mySSAOEffect;
		MotionBlurEffect			myMotionBlurEffect;
		ChromaticAberrationEffect	myChromaticAberrationEffect;
		RadialBlurEffect			myRadialBlurEffect;

		GBufferTexture				mySelectedRenderPass = GBufferTexture::eCount;

	private:	//systems 
		SpriteDrawerSystem* mySpriteDrawer;
		VFXSpriteDrawerSystem* myVFXSpriteDrawer;
		ModelDrawerSystem* myModelDrawer;
		CustomDrawerSystem* myCustomDrawerSystem;
		TransperancyDrawerSystem* myTransperancyDrawerSystem;
		TransformSystem* myTransformSystem;
		AnimationDrawerSystem* myAnimationDrawerSystem;
		CameraSystem* myCameraSystem;
		TextSystem* myTextSystem;
		LightDrawerSystem* myLightDrawerSystem;
		DecalDrawerSystem* myDecalDrawerSystem;
	};


}