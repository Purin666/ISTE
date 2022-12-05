#include "EventHandler.h"

#include <assert.h>

namespace ISTE
{
	void EventHandler::RegisterCallback(EventType aType, std::string aIdentifier, std::function<void(EntityID)> aCallback)
	{

		assert((int)EventType::Count != 0);

		EventCallback callback;
		callback.myCallback = aCallback;
		callback.myIdentifier = aIdentifier;

		myEvents[(int)aType].push_back(callback);
	}
	void EventHandler::RemoveCallback(EventType aType, std::string aIdentifier)
	{
		assert((int)EventType::Count != 0);

		int count = 0;
		for (auto& callback : myEvents[(int)aType])
		{
			if (callback.myIdentifier == aIdentifier)
			{
				myEvents[(int)aType].erase(myEvents[(int)aType].begin() + count);
				break;
			}

			count++;
		}

	}
	void EventHandler::InvokeEvent(EventType aType, EntityID aId)
	{
		assert((int)EventType::Count != 0);

		for (auto& callback : myEvents[(int)aType])
		{
			callback.myCallback(aId);
		}
	}
}