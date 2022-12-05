#pragma once
#include "AbstractTool.h"

#include <string>

namespace ISTE
{
	struct TransformComponent;

	class PlayerCameraTool : public AbstractTool
	{
	public:
		void Init(BaseEditor*) override;
		void Draw() override;

	private:
		void Save(const std::string& aPath, TransformComponent* aTransform);
	};
}