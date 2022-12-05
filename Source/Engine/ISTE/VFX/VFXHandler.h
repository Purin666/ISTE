#pragma once


#include "ISTE/Math/Vec3.h"
#include "ISTE/VFX/VFXSystemDefines.h"

namespace ISTE
{
	struct VFXSystem;

	class VFXHandler
	{
	public:
		const VFXID SpawnVFX(const std::string& aName, const CU::Vec3f aPosition = {}, const CU::Vec3f aRotation = {});
		void SetVFXPosition(const VFXID anId, const CU::Vec3f aPosition);
		void SetVFXRotation(const VFXID anId, const CU::Vec3f aRotation);

		void DeactivateVFX(const VFXID anId);
		void KillVFX(const VFXID anId);
		void KillAllVFX();

		void Update(const float aDeltaTime);
		void CreateRenderCommands();

		inline void	TogglePause() { myPauseFlag = !myPauseFlag; }
		inline void	SetPaused(const bool aFlag) { myPauseFlag = aFlag; }
		inline const bool GetPaused() const { return myPauseFlag; }

	private:
		friend class Engine;
		friend class VFXTool;

		void LoadModelsAndTextures();

		VFXSystem* mySystem;

		bool myPauseFlag = false;
	};
}