#pragma once
#include <wrl/client.h>  
#include <ISTE/Math/Vec.h>

using Microsoft::WRL::ComPtr;
struct ID3D11Buffer;

namespace ISTE
{
	struct Context;
	class ChromaticAberrationEffect
	{
	public:
		enum class Type {
			eRadial,
			eMouseFocus,
			eCount
		};

		struct ChromaticAberrationData {
			float	myRedOffset		=  0.009;
			float	myGreenOffset	=  0.009;
			float	myBlueOffset	= -0.009;
			float gaarb;

			float	myMouseX, myMouseY;
			CU::Vec2f myGarb;

			CU::Vec3f myRadialStrength = {1.f,0.f,0.f};
			float g;
		};

		ChromaticAberrationEffect();
		~ChromaticAberrationEffect();
		bool Init();
		bool Draw();

		void SetEnabled(bool aFlag) { myEnabledFlag = aFlag; };
		void SetType(Type aType) { myAberrationType = aType; };
		ChromaticAberrationData& GetBufferData() { return myBufferData; }

	private:
		friend class DX11;
		Context* myCtx;
		ComPtr<ID3D11Buffer>    myBuffer;
		ChromaticAberrationData	myBufferData;
		Type					myAberrationType = Type::eRadial;
		bool					myEnabledFlag = false;
		void BindBuffers();
		bool InitBuffers();

	};

};
