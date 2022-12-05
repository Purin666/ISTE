#pragma once
#include "AbstractTool.h"
#include "ISTE/ECSB/ECSDefines.hpp"

namespace ISTE
{
	class BaseEditor;
	struct Context;
	class LightTool : public AbstractTool
	{
	public:
		LightTool();
		~LightTool();

		void Draw() override;
		void Init(BaseEditor*) override; 
	private:
		Context* myCtx; 
		BaseEditor* myBaseEditor;

		std::string savedPath = "";

		void DrawDirectionalLight(EntityID aDLIndex);
		void DrawAmbianceLight(EntityID aALIndex);
	};
};