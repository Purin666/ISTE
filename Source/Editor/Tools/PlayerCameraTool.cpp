#include "PlayerCameraTool.h"
#include "imgui/imgui.h"
#include "ISTE/Math/Vec.h"
#include "ISTE/CU/Database.h"
#include "ISTE/Scene/SceneHandler.h"
#include "ISTE/Scene/Scene.h"
#include "ISTE/Context.h"
#include "ISTE/ECSB/PlayerBehaviour.h"
#include "ISTE/Graphics/GraphicsEngine.h"

#include "ISTE/CU/ReadWrite.h"

#include <iostream>
void ISTE::PlayerCameraTool::Init(BaseEditor*)
{
	myToolName = "Player Camera Settings";
}
void ISTE::PlayerCameraTool::Draw()
{
	if (!myActive)
	{
		return;
	}
	ImGui::Begin("PlayerCameraSettings", &myActive, 32);

	Scene& scene = Context::Get()->mySceneHandler->GetActiveScene();
	EntityID playerID = scene.GetPlayerId();

	if (playerID == INVALID_ENTITY)
	{
		ImGui::Text("No Player found");
		ImGui::End();
		return;
	}

	BehaviourHandle& handle = scene.GetBehaviourHandle(playerID);

	PlayerBehaviour* player = handle.GetBehaviour<PlayerBehaviour>();

	TransformComponent& transform = player->GetCameraPositioning();

	CU::Vec3f tempRotation = transform.myEuler.GetAngles();
	ImGui::DragFloat3("Roation", (float*)&tempRotation);
	transform.myEuler.SetRotation(tempRotation);

	ImGui::DragFloat3("Position", (float*)&transform.myPosition);
	ImGui::DragFloat("FOV", &transform.myScale.x, 0.1f, 0.001f, 179.999f);

	player->GetCamera().SetPerspectiveProjection(transform.myScale.x, Context::Get()->myWindow->GetResolution(), 0.5, 5000);

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	const int index = Context::Get()->mySceneHandler->GetActiveSceneIndex();
	const std::string savePath = "../Assets/MiscData/PlayerCamera/PlayerCamera_";
	if (ImGui::Button("Save for this scene"))
		Save(savePath + std::to_string(index) + ".cam", &transform);
	//for (size_t i = 0; i <= GetHighestSceneIndex(); i++) // something like this instead of hardcoded buttons for each
	if (ImGui::Button("Save for Level 0"))
		Save(savePath + "0.cam", &transform);
	if (ImGui::Button("Save for Level 1.1"))
		Save(savePath + "1.cam", &transform);
	if (ImGui::Button("Save for Level 1.2"))
		Save(savePath + "2.cam", &transform);
	if (ImGui::Button("Save for Level 2.1"))
		Save(savePath + "3.cam", &transform);
	if (ImGui::Button("Save for Level 2.2"))
		Save(savePath + "4.cam", &transform);
	if (ImGui::Button("Save for Level 3"))
		Save(savePath + "5.cam", &transform);
	if (ImGui::Button("Save for Level 4"))
		Save(savePath + "6.cam", &transform);

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	static float editorCamFOV = Context::Get()->myGraphicsEngine->GetCamera().GetFov();
	if (ImGui::DragFloat("Editor Cam FOV", &editorCamFOV, 0.1f, 0.001f, 179.999f))
		Context::Get()->myGraphicsEngine->GetCamera().SetFov(editorCamFOV);

	if (ImGui::Button("Copy from Current Camera"))
	{
		CU::Matrix4x4f newTransform = Context::Get()->myGraphicsEngine->GetCamera().GetTransform();
		CU::Vec3f t, r, s;
		newTransform.DecomposeMatrix(r, s, t);


		transform.myEuler.SetRotation(r);
		transform.myScale.x = Context::Get()->myGraphicsEngine->GetCamera().GetFov();
		//if (Context::Get()->mySceneHandler->GetActiveSceneIndex() < 6) // out commented until we use static camera on boss level
		//{
			TransformComponent* playerTransform = scene.GetComponent<TransformComponent>(playerID);
			t -= playerTransform->myPosition;
		//}
		transform.myPosition = t;
	}

	ImGui::End();
}

void ISTE::PlayerCameraTool::Save(const std::string& aPath, TransformComponent* aTransform)
{
	ReadWrite::Que<CU::Vec3f>(aTransform->myEuler.GetAngles());
	ReadWrite::Que<CU::Vec3f>(aTransform->myPosition);
	ReadWrite::Que<float>(aTransform->myScale.x);

	ReadWrite::SaveToFile(aPath);

	std::cout << "--- Saved camera settings to: " << aPath << std::endl;

}