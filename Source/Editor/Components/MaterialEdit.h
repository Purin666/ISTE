#pragma once
#include "ISTE/Scene/Scene.h"
#include "AbstractComponentEdit.h"
#include "ISTE/Graphics/RenderDefines.h"
#include "ISTE/Math/Vec3.h"

namespace ISTE {
	class MaterialEdit : public AbstractComponentEdit {
	public:
		MaterialEdit();
		~MaterialEdit();
		void Draw(ISTE::Scene* aScenePointer, Entity anEntity) override;
	private:
		void MaterialLoop(TextureID aTexList[MAX_MESH_COUNT][MAX_MATERIAL_COUNT], const size_t meshCount);

		int myCurrentMesh = 0;
	};
}