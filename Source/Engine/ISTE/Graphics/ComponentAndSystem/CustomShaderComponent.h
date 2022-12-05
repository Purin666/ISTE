 #pragma once
#include <ISTE/Graphics/Resources/ShaderEnums.h>
#include <ISTE/Math/Matrix4x4.h>

namespace ISTE {

	struct CustomShaderComponent {
		Shaders myShader;
		float myElaspedTime = 0.f;
		float myDuration = 0.f;
		CU::Vec2f myUvChangeOverTimes[3];
		CU::Vec3f mySamples[3];
	};
};