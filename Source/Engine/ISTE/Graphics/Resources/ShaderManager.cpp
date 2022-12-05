#include "ShaderManager.h"
#include <d3d11.h>
#include <fstream>

#include <ISTE/Context.h>
#include <ISTE/Graphics/DX11.h>

#include <ISTE/Graphics/Shaders/cpp/Shader.h>
#include <ISTE/Graphics/Shaders/cpp/SpriteShader.h>
#include <ISTE/Graphics/Shaders/cpp/ModelShader.h>
#include <ISTE/Graphics/Shaders/cpp/AnimatedShader.h>
#include <ISTE/Graphics/Shaders/cpp/VFXModelShader.h>
#include <ISTE/Graphics/Shaders/cpp/VFXSpriteShader.h>

ISTE::ShaderManager::ShaderManager()
{
}

ISTE::ShaderManager::~ShaderManager()
{
	for (auto shader : myShaderList)
		delete shader;
}

bool ISTE::ShaderManager::Init()
{
	myCtx = Context::Get();
	myCtx->myDX11->GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	//shader init
	{
		ModelShader* shader = new ModelShader;
		if (!shader->Init(L"Shaders/Default_VS.cso", L"Shaders/Default_PS.cso"))
			return false;
		myShaderList[(unsigned int)Shaders::eDefaultModelShader] = shader;
	}
	{
		ModelShader* shader = new ModelShader;
		if (!shader->Init(L"Shaders/Default_VS.cso", L"Shaders/Color_PS.cso"))
			return false;
		myShaderList[(unsigned int)Shaders::eColorShader] = shader;
	}
	{
		AnimatedShader* shader = new AnimatedShader;
		if (!shader->Init(L"Shaders/AnimatedDefault_VS.cso", L"Shaders/Normal_PS.cso"))
			return false;
		myShaderList[(unsigned int)Shaders::eAnimatedNormalShader] = shader;
	}
	{
		Sprite3DShader* shader = new Sprite3DShader;
		if (!shader->Init(L"Shaders/3DSprite_VS.cso", L"Shaders/3DSprite_PS.cso"))
			return false;
		myShaderList[(unsigned int)Shaders::eSpriteShader] = shader;
	}


	// VFX Shaders
	{
		VFXModelShader* shader = new VFXModelShader;
		if (!shader->Init(L"Shaders/VFXModel_VS.cso", L"Shaders/VFXModel_PS.cso"))
			return false;
		myShaderList[(unsigned int)Shaders::eDefaultVFXModelShader] = shader;
	}
	// damaged
	{
		AnimatedShader* shader = new AnimatedShader;
		if (!shader->Init(L"Shaders/AnimatedDefault_VS.cso", L"Shaders/PlayerDamaged_PS.cso"))
			return false;
		myShaderList[(unsigned int)Shaders::ePlayerDamaged] = shader;
	}
	{
		AnimatedShader* shader = new AnimatedShader;
		if (!shader->Init(L"Shaders/AnimatedDefault_VS.cso", L"Shaders/EnemyDamaged_PS.cso"))
			return false;
		myShaderList[(unsigned int)Shaders::eEnemyDamaged] = shader;
	}
	// Player Abilities
	{
		VFXModelShader* shader = new VFXModelShader;
		if (!shader->Init(L"Shaders/VFXModel_VS.cso", L"Shaders/MagicArmor_PS.cso"))
			return false;
		myShaderList[(unsigned int)Shaders::eMagicArmor] = shader;
	}
	{
		VFXModelShader* shader = new VFXModelShader;
		if (!shader->Init(L"Shaders/VFXModel_VS.cso", L"Shaders/AreaOfEffectAtPlayer_PS.cso"))
			return false;
		myShaderList[(unsigned int)Shaders::eAreaOfEffectAtPlayer] = shader;
	}
	//LoD
	{
		ModelShader* shader = new ModelShader;
		if (!shader->Init(L"Shaders/Default_VS.cso", L"Shaders/LoDFade_PS.cso"))
			return false;
		myShaderList[(unsigned int)Shaders::eLoDFade] = shader;
	}
	 


	// Fullscreen Shaders
	{
		auto& gaussianH		= myFullscreenEffectList[(unsigned int)eFullscreenEffects::ePPGaussianH];
		auto& gaussianV		= myFullscreenEffectList[(unsigned int)eFullscreenEffects::ePPGaussianV];
		auto& ssao			= myFullscreenEffectList[(unsigned int)eFullscreenEffects::ePPSSAO];
		auto& tonemap		= myFullscreenEffectList[(unsigned int)eFullscreenEffects::ePPTonemap];
		auto& downScale		= myFullscreenEffectList[(unsigned int)eFullscreenEffects::ePPDownSample];
		auto& upScale		= myFullscreenEffectList[(unsigned int)eFullscreenEffects::ePPUpSample];  
		auto& copy			= myFullscreenEffectList[(unsigned int)eFullscreenEffects::ePPCopy];
		auto& fog			= myFullscreenEffectList[(unsigned int)eFullscreenEffects::ePPFog];
		auto& dirAmbLight	= myFullscreenEffectList[(unsigned int)eFullscreenEffects::ePPDirAmbLight]; 
		auto& gbufferCopy	= myFullscreenEffectList[(unsigned int)eFullscreenEffects::ePPGbufferCopy];
		auto& motionBlur	= myFullscreenEffectList[(unsigned int)eFullscreenEffects::ePPMotionBlur];
		auto& chromAbeRad	= myFullscreenEffectList[(unsigned int)eFullscreenEffects::ePPChromaticAbeRadial];
		auto& chromAbeMouse = myFullscreenEffectList[(unsigned int)eFullscreenEffects::ePPChromaticAbeMouseFocus];
		auto& radialBlur	= myFullscreenEffectList[(unsigned int)eFullscreenEffects::ePPRadialBlur];
		
		
		if (!gaussianH.Init("Shaders/PostprocessGaussianH_PS.cso"))
			return false;
		if (!gaussianV.Init("Shaders/PostprocessGaussianV_PS.cso"))
			return false;
		if (!ssao.Init("Shaders/PostProcessSSAO_PS.cso"))
			return false; 
		if (!motionBlur.Init("Shaders/PostProcessMotionBlur_PS.cso"))
			return false;
		if (!radialBlur.Init("Shaders/PostProcessRadialBlur_PS.cso"))
			return false;
		if (!chromAbeRad.Init("Shaders/PostProcessChromaticAberrationRadial_PS.cso"))
			return false;
		if (!chromAbeMouse.Init("Shaders/PostProcessChromaticAberrationMouseFocus_PS.cso"))
			return false;
		if (!tonemap.Init("Shaders/PostProcessTonemap_PS.cso"))
			return false;
		if (!downScale.Init("Shaders/PostProcessDownSample_PS.cso"))
			return false;
		if (!upScale.Init("Shaders/PostProcessUpSample_PS.cso"))
			return false;
		if (!copy.Init("Shaders/PostprocessCopy_PS.cso"))
			return false;
		if (!fog.Init("Shaders/ExpFog_PS.cso"))
			return false;
		if (!dirAmbLight.Init("Shaders/Directional_Ambiance_HemisphericAmbient_PS.cso"))
			return false;
		if (!gbufferCopy.Init("Shaders/CopyGBuffer_PS.cso"))
			return false;
	}
	return true;
}

void ISTE::ShaderManager::Draw(Shaders aShader, size_t aEntity)
{
	myShaderList[(int)aShader]->Render(aEntity);
}

void ISTE::ShaderManager::BindShader(Shaders aShader)
{
	myShaderList[(int)aShader]->BindShader();
}
