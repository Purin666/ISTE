#include "NavMeshTool.h"

#include "imgui/imgui.h"
#include "imguizmo/ImGuizmo.h"

#include "ISTE/Context.h"
#include "ISTE/Graphics/GraphicsEngine.h"
#include "ISTE/Graphics/DebugDrawer.h"
#include "ISTE/Navigation/NavMeshDefines.h"
#include "ISTE/Scene/Scene.h"
#include "ISTE/Scene/SceneHandler.h"
#include "ISTE/CU/InputHandler.h"

#include "ISTE/Graphics/ComponentAndSystem/TransformComponent.h"
#include "ISTE/Graphics/ComponentAndSystem/SpriteDrawerSystem.h"
#include "ISTE/Graphics/ComponentAndSystem/BillboardRenderCommand.h"
#include "ISTE/Graphics/Resources/TextureManager.h"

#include "ISTE/CU/Geometry/Intersection.h"

#include "Commands/AbstractCommand.h"
#include "Commands/VariableCommand.h"
#include "Commands/CommandManager.h"

#include "ISTE/Events/EventHandler.h"
#include "ISTE/Navigation/NavMeshUtilities.h"


void ISTE::NavMeshTool::Init(BaseEditor*)
{
	myToolName = "Navigation";

	myIcon = Context::Get()->myTextureManager->LoadTexture(L"../EngineAssets/Images/NavMeshIcon.dds", true);

	Context::Get()->myEventHandler->RegisterCallback(EventType::SceneLoaded, "NavigationTool", [this](EntityID) { SceneLoaded(); });
	Context::Get()->myEventHandler->RegisterCallback(EventType::SceneDestroyed, "NavigationTool", [this](EntityID) { SceneDestroyed(); });
}

void ISTE::NavMeshTool::Draw()
{
	// saftey checks
	{
		if (!myActive)
			return;
		if (myNavMesh == nullptr)
			return;
		if (!myNavMesh->vertices.empty())
			mySelectedVertex %= myNavMesh->vertices.size();
	}

	if (Context::Get()->myInputHandler->IsKeyUp(VK_LBUTTON))
		Pick();

	DrawGizmo();
	DrawEdgesAndConnections();
	DrawBillboards();
	DrawSpheres();

	ImGui::PushID(myToolName.c_str());
	ImGui::Begin(myToolName.c_str(), &myActive);

	if (!myNavMesh->vertices.empty())
	{
		if (ImGui::Button("Save"))
		{
			const Scene& scene = Context::Get()->mySceneHandler->GetActiveScene();
			const std::string path = "../Assets/NavMesh/" + scene.GetName() + "NavMesh.obj";
			NavMesh::SaveNavMesh(path, *myNavMesh);
		}
		ImGui::Text("Selected");
		const CU::Vec3f p = myNavMesh->vertices[mySelectedVertex];
		std::stringstream ss; ss << p.x << ", " << p.y << ", " << p.z;
		ImGui::Text(("\tVertice: " + std::to_string(mySelectedVertex)).c_str());
		ImGui::Text(("\tSelected Position: " + ss.str()).c_str());
		if (ImGui::Button("Kill"))
		{
			TryKill(mySelectedVertex);
		}
	}
	else
		ImGui::Text("No NavMesh");

	ImGui::End();
	ImGui::PopID();

	KillVertex();
}

void ISTE::NavMeshTool::CreateSpheres()
{
	mySpheres.clear();

	const size_t size = myNavMesh->vertices.size();
	mySpheres.resize(size);

	for (size_t i = 0; i < size; i++)
		mySpheres[i].InitWithCenterAndRadius(myNavMesh->vertices[i], 0.2f);
}

void ISTE::NavMeshTool::SceneLoaded()
{
	Scene& scene = Context::Get()->mySceneHandler->GetActiveScene();
	myNavMesh = &scene.GetNavMesh();
	CreateSpheres();
}
void ISTE::NavMeshTool::SceneDestroyed()
{
	myNavMesh = nullptr;
	mySpheres.clear();
}

void ISTE::NavMeshTool::TryKill(const size_t aVertexIndex)
{
	size_t count = 0;
	size_t triangleIndex = 0;
	auto& tris = myNavMesh->triangles;
	for (size_t i = 0; i < tris.size(); i++)
	{
		for (const auto& k : tris[i].vertexIndices)
		{
			if (k == aVertexIndex)
			{
				count++;
				triangleIndex = tris[i].index;
			}
		}
	}
	if (count > 1)
		return;

	myKillQueue.push({ triangleIndex, aVertexIndex });
}

void ISTE::NavMeshTool::KillVertex()
{
	while (!myKillQueue.empty())
	{
		const KillPair pair = myKillQueue.front();
		myKillQueue.pop();

		auto& tris = myNavMesh->triangles;
		auto& verts = myNavMesh->vertices;
		// since the vertex we're removing is exlusive for one triangle there can only be one connection to said triangle we're removing
		auto& tri = myNavMesh->triangles[pair.triangleIndex];
		auto& other = myNavMesh->triangles[tri.connections.front()];
		// but the connected triangle can have up to two more connections
		auto& oc = other.connections;
		for (size_t i = 0; i < oc.size(); i++)
		{
			if (oc[i] = pair.triangleIndex)
			{
				// remove connection
				std::swap(oc[i], oc.back());
				oc.pop_back();
				tris[pair.triangleIndex].connections.clear();
				break;
			}
		}
	}
}

void ISTE::NavMeshTool::Pick()
{
	ScreenHelper::Ray ray = myScreenHelper.GetRayFromPoint(myScreenHelper.GetMouseRelativePosition(), Context::Get()->myWindow->GetResolution().myValue);
	CU::Ray<float> realRay;
	for (size_t i = 0; i < mySpheres.size(); i++)
	{
		realRay.InitWithOriginAndDirection(ray.myOrigin, ray.myDir);
		if (CU::IntersectionSphereRay(mySpheres[i], realRay))
		{
			mySelectedVertex = i;
			return;
		}
	}
}

void ISTE::NavMeshTool::DrawGizmo()
{
	CU::Vec3f* position = &myNavMesh->vertices[mySelectedVertex];

	if (position == nullptr)
		return;

	ImGui::SetItemDefaultFocus();

	ImGuiIO& io = ImGui::GetIO();
	{
		ImGuizmo::SetOrthographic(false);
		ImGuizmo::SetDrawlist(ImGui::GetBackgroundDrawList());

		WindowsWindow* window = Context::Get()->myWindow;

		ImGuizmo::SetRect(0.0f, 0.0f, (float)window->GetWidth(), (float)window->GetHeight());

		Camera& camera = Context::Get()->myGraphicsEngine->GetCamera();

		CU::Matrix4x4f view = CU::Matrix4x4f::FastInverse(camera.GetTransform());
		CU::Matrix4x4f proj = camera.GetProjectionMatrix();

		CU::Matrix4x4f transform = CU::Matrix4x4f::CreateTranslationMatrix(*position);

		static bool inUse = false;
		if (ImGuizmo::IsOver() && io.MouseClicked[0])
		{
			myOriginPos = *position;
			inUse = true;
		}

		ImVec2 oldPos = io.MousePos;
		ImVec2 oldPosPrev = io.MousePosPrev;

		ImGuizmo::Manipulate(
			&view(1, 1),
			&proj(1, 1),
			ImGuizmo::OPERATION::TRANSLATE,
			ImGuizmo::MODE::WORLD,
			&transform(1, 1), nullptr
		);

		if (inUse == true && io.MouseReleased[0])
		{
			inUse = false;
			CommandManager::DoCommand(new VariableCommand<CU::Vec3f>(position, myOriginPos, *position));
		}
		if (ImGuizmo::IsUsing())
		{
			*position = transform.GetTranslationV3();
		}
		io.MousePos = oldPos;
		io.MousePosPrev = oldPosPrev;
	}
	if (ImGuizmo::IsUsing())
		mySpheres[mySelectedVertex].center = *position;
}

void ISTE::NavMeshTool::DrawEdgesAndConnections()
{
	DebugDrawer& drawer = Context::Get()->myGraphicsEngine->GetDebugDrawer();
	size_t i, iTo;
	LineCommand edge, conn;
	edge.myColor = CU::Vec3f(0.9f, 0.5f, 0.3f);
	conn.myColor = CU::Vec3f(0.7f, 0.7f, 0.0f);

	for (auto& tri : myNavMesh->triangles)
	{
		for (i = 1; i <= 3; ++i)
		{
			edge.myFromPosition = *tri.vertices[i - 1];
			iTo = i % 3;
			edge.myToPosition = *tri.vertices[iTo];
			drawer.AddDynamicLineCommand(edge);
		}

		for (const int connIndex : tri.connections)
		{
			conn.myFromPosition = tri.Center();
			conn.myToPosition = myNavMesh->triangles[connIndex].Center();
			drawer.AddDynamicLineCommand(conn);
		}
	}
}

void ISTE::NavMeshTool::DrawBillboards()
{
	SpriteDrawerSystem* drawer = Context::Get()->mySystemManager->GetSystem<SpriteDrawerSystem>();
	BillboardCommand com;
	com.myTransform.GetRow(1) *= 0.4f;
	com.myTransform.GetRow(2) *= 0.4f;
	com.myTextures = myIcon;

	for (size_t i = 0; i < myNavMesh->vertices.size(); i++)
	{
		com.myTransform.GetTranslationV3() = myNavMesh->vertices[i];
		drawer->AddBillboardCommand(com);
	}
}

void ISTE::NavMeshTool::DrawSpheres()
{
	DebugDrawer& drawer = ISTE::Context::Get()->myGraphicsEngine->GetDebugDrawer();
	const size_t res = 32;

	LineCommand lc;
	lc.myColor = { 0.9f, 0.5f, 0.3f };

	std::vector<CU::Vec3f> xy, yz, zx;
	xy.resize(res);
	yz.resize(res);
	zx.resize(res);

	size_t i;
	CU::Vec2f v;
	const float mod = float(360 / res);

	for (const CU::Sphere<float>& s : mySpheres)
	{
		for (i = 0; i < res; ++i)
		{
			v = CU::GetVectorFromRadians((float)i * mod * ISTE::DegToRad) * s.radius;

			xy[i].x = v.x + s.center.x;
			xy[i].y = v.y + s.center.y;
			xy[i].z = s.center.z;

			yz[i].x = s.center.x;
			yz[i].y = v.x + s.center.y;
			yz[i].z = v.y + s.center.z;

			zx[i].x = v.y + s.center.x;
			zx[i].y = s.center.y;
			zx[i].z = v.x + s.center.z;
		}

		for (i = 0; i < res - 1; ++i)
		{
			lc.myFromPosition = yz[i];
			lc.myToPosition = yz[i + 1];
			drawer.AddDynamicLineCommand(lc);

			lc.myFromPosition = zx[i];
			lc.myToPosition = zx[i + 1];
			drawer.AddDynamicLineCommand(lc);

			lc.myFromPosition = xy[i];
			lc.myToPosition = xy[i + 1];
			drawer.AddDynamicLineCommand(lc);
		}
		lc.myFromPosition = yz.back();
		lc.myToPosition = yz[0];
		drawer.AddDynamicLineCommand(lc);

		lc.myFromPosition = zx.back();
		lc.myToPosition = zx[0];
		drawer.AddDynamicLineCommand(lc);

		lc.myFromPosition = xy.back();
		lc.myToPosition = xy[0];
		drawer.AddDynamicLineCommand(lc);
	}
}
