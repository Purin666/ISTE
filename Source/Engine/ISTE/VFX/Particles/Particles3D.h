#pragma once

#include "ISTE/Math/Vec3.h"

namespace ISTE
{
	struct Emitter3DSystem;

	void		SpawnEmitter				(Emitter3DSystem& aSystem, const CU::Vec3f& aPosition = {}, const CU::Vec3f& aRotation = {}); // returns ID of new emitter on success; returns 0 on failure
	void		SetEmitterPosition			(Emitter3DSystem& aSystem, const CU::Vec3f& aNewPosition);
	void		SetEmitterRotation			(Emitter3DSystem& aSystem, const CU::Vec3f& someEulerAngles);
	void		DeactivateEmitter			(Emitter3DSystem& aSystem); // stops emitter from spawning new particles, and destroys it once all its particles are gone
	void		KillEmitter					(Emitter3DSystem& aSystem); // destroys emitter immediately along with all its particles 
	void		UpdateEmittersAndParticles	(Emitter3DSystem& aSystem, const float aDeltaTime); // also spawns particles
	void		CreateRenderCommands		(Emitter3DSystem& aSystem);
	void		LoadParticleTextures		(Emitter3DSystem& aSystem);
}
