
#include "ISTE/ECSB/ECSDefines.hpp"

#include "ISTE/Math/Vec3.h"
#include <string>
#include <queue>

namespace ISTE
{
	struct ModelVFXSystem;

	class ModelVFXHandler
	{
	public:

		const EntityID  Spawn		(const std::string& aName, const CU::Vec3f& aPosition = {}, const CU::Vec3f& someEulerAngles = {});
		void			Attach		(const EntityID aVFX, const EntityID anOther);
		void			SetPosition	(const EntityID aVFX, const CU::Vec3f& aPosition);
		void			SetRotation	(const EntityID aVFX, const CU::Vec3f& someEulerAngles);
		void			Kill		(const EntityID aVFX);
		void			KillAll		();

		void			Update(const float aTimeDelta);

		void			LoadModelsAndTextures();

		inline void TogglePause() { myPauseFlag = !myPauseFlag; }
		inline void	SetPaused(const bool aFlag) { myPauseFlag = aFlag; }
		inline const bool GetPaused() const { return myPauseFlag; }


	private:
		friend class Engine;
		friend class ModelVFXTool;
		friend class VFXTool;

		void UpdateInstances(const float aTimeDelta);
		void UpdateComponents();

		void LoadModels();
		void LoadTextures();

	private:
		ModelVFXSystem* mySystem;
		std::queue<EntityID> myKillQueue;

		bool myPauseFlag = false;
	};
}