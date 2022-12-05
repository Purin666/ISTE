#pragma once 
#include <string> 
#include "ShaderEnums.h"
#include <ISTE/Graphics/Shaders/cpp/FullscreenEffect.h>

namespace ISTE
{
	struct Context;
	class Shader;
	class ShaderManager
	{
	public:
		ShaderManager();
		~ShaderManager();

		bool Init();

		void Draw(Shaders, size_t aEntity);
		void BindShader(Shaders);
		inline FullscreenEffect& GetFullscreenEffect(eFullscreenEffects aType) { return myFullscreenEffectList[(unsigned long)aType]; }


	private:
		Shader* myShaderList[(unsigned long)Shaders::eCount] = { nullptr };
		FullscreenEffect myFullscreenEffectList[(unsigned long)eFullscreenEffects::eCount];
		
		Context* myCtx;
	};
}