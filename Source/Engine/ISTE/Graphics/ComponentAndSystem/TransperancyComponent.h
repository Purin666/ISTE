#pragma once
#include <ISTE/Graphics/Resources/ShaderEnums.h>

namespace ISTE {
	struct TransperancyComponent {
		float myTransperancyModifier = 1.f;
		Shaders myShader = Shaders::eCount;
	};
}