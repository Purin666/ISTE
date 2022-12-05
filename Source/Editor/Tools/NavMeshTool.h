#pragma once
#include "AbstractTool.h"

#include "ISTE/CU/Geometry/Intersection.h"
#include "ISTE/CU/RayFromScreen.h"

#include <windef.h>
#include <queue>

namespace ISTE
{
	struct KillPair
	{
		size_t triangleIndex;
		size_t vertexIndex;
	};
	namespace NavMesh { class NavMesh; }

	class NavMeshTool : public AbstractTool
	{
	public:
		void Init(BaseEditor*) override;
		void Draw();

	private:
		void CreateSpheres();
		void SceneLoaded();
		void SceneDestroyed();

		void TryKill(const size_t);
		void KillVertex();

		void Pick();

		void DrawGizmo();
		void DrawEdgesAndConnections();
		void DrawBillboards();
		void DrawSpheres();

	private:
		ScreenHelper myScreenHelper;
		NavMesh::NavMesh* myNavMesh;
		std::vector<CU::Sphere<float>> mySpheres;
		unsigned short myIcon;
		size_t mySelectedVertex = 0;
		CU::Vec3f myOriginPos;
		std::queue<KillPair> myKillQueue;
	};
}