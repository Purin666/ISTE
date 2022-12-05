#pragma once

namespace ISTE
{ 
	enum class RasterizerState
	{
		eBackFaceCulling,
		eFrontFaceCulling,
		eNoFaceCulling,
		eWireFrameMode,
		eBiasedRaster,	//only used for shadow mapping
		eCount
	};

	enum class ReadWriteState
	{
		eRead,
		eWrite,
		eCount
	};

	enum class DepthState
	{
		eDepthLess,
		eDepthLessOrEqual,
		eDepthGreater,
		eDepthGreaterOrEqual,
		eAlwaysPass,
		eCount
	};


	enum class BlendState
	{
		eDisabled,
		eAlphaBlend,
		eAddativeBlend,
		eAddAndFadeBackgroundBlend,
		eCount
	};

	enum class SamplerState
	{
		ePoint,
		eBiliniear,
		eTriLinear,
		eAnisotropic,
		eCount
	};

	enum class AdressMode
	{
		eClamp,
		eMirror,
		eWrap,
		eCount
	};
}