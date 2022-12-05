#pragma once

#include "ISTE/ECSB/ECSDefines.hpp"

#include <functional>
#include <vector>

namespace ISTE
{
	enum class EventType
	{
		EntityMoved,
		EntityParented,
		EntityParentedToSpace,
		EntityUnParented,
		EntityCreated,
		EntityDestroyed,
		EntityActivated,
		EntityDeactivated,

		SceneLoaded,
		SceneDestroyed,

		PlayerTookDamage,
		PlayerAbsorbedDamage,
		PlayerSpentMana,
		PlayerGainExperience,
		PlayerGainLevel,
		PlayerUsedAbility,

		PlayerMoveDistance,
		PlayerTeleportedDistance,

		MinionTookDamage,
		BossTookDamage,

		PlayerDied,
		TimDied,
		TimEliteDied,
		BorisDied,
		BorisEliteDied,
		BobDied,
		PotDied,

		LevelCompleted,


		Count
	};



	struct EventCallback
	{
		std::function<void(EntityID)> myCallback;
		std::string myIdentifier;
	};

	//should have a more dynamic system for the future
	//like registering an event including it's paramaters 
	class EventHandler
	{
	public:
		void RegisterCallback(EventType aType, std::string aIdentifier, std::function<void(EntityID)> aCallback);
		void RemoveCallback(EventType aType, std::string aIdentifier);

		//if entityid isn't expected the id could be garbage value
		void InvokeEvent(EventType aType, EntityID aId);

	private:
		std::vector<EventCallback> myEvents[(int)EventType::Count];

		
	};
}