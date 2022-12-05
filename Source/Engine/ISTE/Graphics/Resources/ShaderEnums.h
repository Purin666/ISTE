#pragma once
namespace ISTE
{
	enum class Shaders
	{
		eDefaultModelShader,
		eColorShader,
		eSpriteShader,
		eAnimatedNormalShader,

		ePlayerDamaged,
		eEnemyDamaged,

		// VFX shaders
		eDefaultVFXModelShader,

		eMagicArmor,
		eAreaOfEffectAtPlayer,

		eLoDFade,

		/* Uppgift 14 */
		eBall,
		eBeam,
		eCircle,
		eSworl,

		eCount
	}; 

	enum class eFullscreenEffects
	{
		ePPGaussianH,
		ePPGaussianV,
		ePPSSAO,
		ePPMotionBlur,
		ePPRadialBlur,
		ePPChromaticAbeRadial,
		ePPChromaticAbeMouseFocus,
		ePPTonemap,
		ePPDownSample,
		ePPUpSample,
		ePPFog,
		ePPCopy,
		ePPGbufferCopy,

		ePPDirAmbLight,
		
		eCount
	};
}