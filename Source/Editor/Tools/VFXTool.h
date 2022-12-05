#pragma once
#include "AbstractTool.h"

#include "ISTE/ECSB/ECSDefines.hpp"

#include <vector>

namespace ISTE
{
	struct Context;
	struct VFXData;

	enum class SelectedToolVFX
	{
		Package,
		Model,
		Emitter3D,
		Emitter2D,

		Count
	};
	enum class SelectedEditVFX
	{
		Models,
		Emitters3D,
		Emitters2D,
		Decals,
		PointLights,
		SpotLights,

		Count
	};

	class VFXTool : public AbstractTool
	{
	public:
		void Init(BaseEditor*) override;
		void Draw() override;

	private:
		void DrawPackageTool();
		void DrawModelTool();
		void DrawEmitter3DTool();
		void DrawEmitter2DTool();

		// functions for package;
		void DrawTopConfig();
		void DrawSelection();
		void DrawControl(VFXData& data, std::vector<std::string>& names, std::string& selected, std::string& newNameStatic);

		Context* myCtx = nullptr;

		SelectedToolVFX mySelectedTool = SelectedToolVFX::Package;
		SelectedEditVFX mySelectedEdit = SelectedEditVFX::Count;

		// package;
		std::string mySelectedPackageData = "N/A";
		std::string mySelectedModel = "N/A";
		std::string mySelectedEmitter = "N/A";
		int myVFXId = -1;

		// model
		std::string mySelectedModelData = "N/A";
		EntityID myModelId = INVALID_ENTITY;

		// emitter 3d
		std::string mySelectedEmitter3DData = "N/A";
		std::vector<int> myEmitter3DIds;

		// emitter 2d
		std::string mySelectedEmitter2DData = "N/A";
		int myEmitter2DId = -1;

	};
}