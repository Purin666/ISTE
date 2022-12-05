#pragma once

#include <string>

namespace ISTE
{
	struct EmitterComponent
	{
		std::string myEmitterType;
		int myEmitterId = -1;

		~EmitterComponent() = default;
	};
}