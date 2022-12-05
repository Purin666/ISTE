#pragma once

#include "ISTE/Math/Vec3.h"
#include "ISTE/ECSB/ECSDefines.hpp"

#include <string>
#include <vector>
#include <queue>

#include "ISTE/CU/Geometry/AABB3D.h"
#include "ISTE/CU/Geometry/Sphere.h"

namespace ISTE
{
	struct Emitter3DSystem;
	
	class Sprite3DParticleHandler
	{
	public:
		/* Returns ID of new emitter on success; returns 0 on failure. */
		const int	SpawnEmitter				(const std::string& aName, const CU::Vec3f aPosition = {}, const CU::Vec3f someEulerAngles = {});
		void		SetEmitterPosition			(const int anEmitterId, const CU::Vec3f aNewPosition);
		void		SetEmitterRotation			(const int anEmitterId, const CU::Vec3f someEulerAngles);
		/* Disable emitter to spawn new particles */
		void		DeactivateEmitter			(const int anEmitterId);
		/* Enable emitter to spawn new particles */
		void		ActivateEmitter				(const int anEmitterId);
		/* Kills emitter immediately along with all its particles. */
		void		KillEmitter					(const int anEmitterId); //  
		/* Kills all emitter immediately along with all their particles. */
		void		KillAllEmitters				();
		void		UpdateEmittersAndParticles	(const float aDeltaTime); // also spawns particles
		void		CreateRenderCommands		();

		/* Returns all active emitters */
		std::vector<int> GetEmitters(const std::string& aTypeName = "");
		/* Returns all active emitters inside AABB3D*/
		std::vector<int> GetEmitters(const CU::AABB3D<float>& aBox, const std::string& aTypeName = "");
		/* Returns all active emitters inside Sphere*/
		std::vector<int> GetEmitters(const CU::Sphere<float>& aSphere, const std::string& aTypeName = "");

		inline void	TogglePause() { myPauseFlag = !myPauseFlag; }
		inline void	SetPaused(const bool aFlag) { myPauseFlag = aFlag; }
		inline const bool GetPaused() const { return myPauseFlag; }

		// Testing
		void PushEmitters(const std::vector<int> someEmitterIds, const CU::Vec3f& sourcePosition, const float sourceForce);

	private:
		friend class Engine;
		friend class Particle3DTool;
		friend class VFXTool;

		void LoadParticleTextures();
		Emitter3DSystem* mySystem;
		std::queue<int> myKillQueue;

		bool myPauseFlag = false;
	};
}
