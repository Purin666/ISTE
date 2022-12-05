#pragma once

#include <string>

namespace ISTE
{
	struct AttackValueComponent
	{
		AttackValueComponent(AttackValueComponent& aLeft) {
			memcpy(this, &aLeft, sizeof(AttackValueComponent));
		};
		AttackValueComponent() {};
		~AttackValueComponent() {};
		AttackValueComponent& operator= (AttackValueComponent& other)
		{
			memcpy(this, &other, sizeof(AttackValueComponent));
			return *this;
		}

		float myAttackValue = 0.f;

		std::string myTag = "";
		std::string myExtraInfo = "";
		std::string myIdentifier = "";
	};
}