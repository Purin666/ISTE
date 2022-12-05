#include "Gizmos.h"

#include "imgui/imgui.h"
#include "imguizmo/ImGuizmo.h"

#include "ISTE/Context.h"
#include "ISTE/WindowsWindow.h"
#include "ISTE/Graphics/Camera.h"
#include "ISTE/Graphics/GraphicsEngine.h"

#include "ISTE/Scene/SceneHandler.h"
#include "ISTE/Scene/Scene.h"
#include "ISTE/CU/InputHandler.h"
#include "ISTE/Graphics/ComponentAndSystem/TransformComponent.h"
#include "Commands/AbstractCommand.h"
#include "Commands/VariableCommand.h"
#include "Commands/CommandManager.h"

#include "ISTE/CU/Geometry/Intersection.h"

#include "ISTE/CU/RayFromScreen.h"

#include "ISTE/CU/MemTrack.hpp"

#include <Windows.h>

namespace ISTE
{
	Gizmos::Gizmos()
	{
		char* user = getenv("username");
		std::string name(user);

		if (name == "olivia.xu")
		{
			myTranslationKey = 'T';
			myRotationKey = 'R';
			myScaleKey = 'S';
		}

	}
	void Gizmos::CheckInput(bool aDisplayActive)
	{

		CU::InputHandler* handler = Context::Get()->myInputHandler;

		bool windowFocused = ImGui::IsAnyWindowFocused();

		bool ctrl = handler->IsKeyHeldDown(VK_CONTROL);
		bool rMB = handler->IsKeyHeldDown(MK_RBUTTON);


		if (ctrl && handler->IsKeyHeldDown('X') && handler->IsKeyDown('U'))
		{
			myTranslationKey = 'W';
			myRotationKey = 'E';
			myScaleKey = 'R';
		}

		if (((windowFocused && aDisplayActive) || !windowFocused) && !rMB)
		{
			if (handler->IsKeyDown(myTranslationKey))
			{
				mySelectedOption = GizmosOptions::Translate;
			}
			else if (handler->IsKeyDown(myRotationKey))
			{
				mySelectedOption = GizmosOptions::Rotate;
			}
			else if (handler->IsKeyDown(myScaleKey))
			{
				mySelectedOption = GizmosOptions::Scale;
			}
			else if (handler->IsKeyDown('Q'))
			{
				if (mySpaceOption == GizmosOptions::World)
					mySpaceOption = GizmosOptions::Local;
				else if (mySpaceOption == GizmosOptions::Local)
					mySpaceOption = GizmosOptions::World;
			}

			mySnap[0] = 0.0f;
			mySnap[1] = 0.0f;
			mySnap[2] = 0.0f;

			if (ctrl)
			{
				mySnap[0] = mySnapValue;
				mySnap[1] = mySnapValue;
				mySnap[2] = mySnapValue;
			}
		}
	}

	bool ISTE::Gizmos::Draw(RECT aDisplayRect, bool aDisplayActive)
	{

		if (!Context::Get()->mySceneHandler->GetActiveScene().IsEntityIDValid(myEntity))
			return false;

		TransformComponent* tComp = Context::Get()->mySceneHandler->GetActiveScene().GetComponent<TransformComponent>(myEntity);

		if (tComp == nullptr)
			return false;

		ImGui::SetItemDefaultFocus();

		auto& io = ImGui::GetIO();
		{

			ImGuizmo::SetOrthographic(false);
			ImGuizmo::SetDrawlist(ImGui::GetBackgroundDrawList());

			WindowsWindow* window = Context::Get()->myWindow;

			ImGuizmo::SetRect(0.0f, 0.0f, (float)window->GetWidth(), (float)window->GetHeight());

			Camera& camera = Context::Get()->myGraphicsEngine->GetCamera();

			CU::Matrix4x4f view = CU::Matrix4x4f::FastInverse(camera.GetTransform());
			CU::Matrix4x4f proj = camera.GetProjectionMatrix();

			CU::Matrix4x4f transform = tComp->myCachedTransform;


			static bool inUse = false;

			

			if (ImGuizmo::IsOver() && io.MouseClicked[0]) //this would use standard coordinates
			{
				myOriginPos = tComp->myPosition;
				myOriginRot = tComp->myEuler;
				myOriginSize = tComp->myScale;
				inUse = true;
			}

			//second calculation accounting for displaySize

			ImVec2 oldPos = io.MousePos;
			ImVec2 oldPosPrev = io.MousePosPrev;


			if (aDisplayActive)
			{
				ScreenHelper helper;

				POINT mousePos = helper.GetMouseRelativePosition();
				CU::Vec2Ui res = Context::Get()->myWindow->GetResolution();

				int x = mousePos.x - aDisplayRect.left;
				int y = mousePos.y - aDisplayRect.top;

				int xPrev = oldPosPrev.x - aDisplayRect.left;
				int yPrev = oldPosPrev.y - aDisplayRect.top;

				float percX = (float)x / (float)(aDisplayRect.right - aDisplayRect.left);
				float percY = (float)y / (float)(aDisplayRect.bottom - aDisplayRect.top);

				mousePos = { (int)(res.x * percX), (int)(res.y * percY) };

				float percXPrev = (float)xPrev / (float)(aDisplayRect.right - aDisplayRect.left);
				float percYPrev = (float)yPrev / (float)(aDisplayRect.bottom - aDisplayRect.top);

				//guess
				ImVec2 prevPos = { res.x * percXPrev, res.y * percYPrev };

				io.MousePos = { (float)mousePos.x, (float)mousePos.y };
				io.MousePosPrev = prevPos;

				if (ImGuizmo::IsOver() && io.MouseClicked[0])
				{
					myOriginPos = tComp->myPosition;
					myOriginRot = tComp->myEuler;
					myOriginSize = tComp->myScale;
					inUse = true;
				}
			}
			ImGuizmo::Manipulate(
				&view(1, 1),
				&proj(1, 1),
				(ImGuizmo::OPERATION)mySelectedOption,
				(ImGuizmo::MODE)mySpaceOption,
				&transform(1, 1), nullptr, mySnap
			);

			if (inUse == true && io.MouseReleased[0]) {
				inUse = false;
				switch (mySelectedOption)
				{
				case GizmosOptions::Translate:
				{
					CommandManager::DoCommand(new VariableCommand<CU::Vec3f>(&tComp->myPosition, myOriginPos, tComp->myPosition));
					break;
				}
				case GizmosOptions::Scale:
				{
					CommandManager::DoCommand(new VariableCommand<CU::Vec3f>(&tComp->myScale, myOriginSize, tComp->myScale));
					break;
				}
				case GizmosOptions::Rotate:
				{
					CommandManager::DoCommand(new VariableCommand<CU::Euler>(&tComp->myEuler, myOriginRot, tComp->myEuler));
					break;
				}
				}
			}
			if (ImGuizmo::IsUsing())
			{
				EntityID par = Context::Get()->mySceneHandler->GetActiveScene().GetParent(myEntity);

				if (par != INVALID_ENTITY)
				{
					TransformComponent* pCMP = Context::Get()->mySceneHandler->GetActiveScene().GetComponent<TransformComponent>(par);

					if (pCMP != nullptr)
						transform = transform * CU::Matrix4x4f::Inverse(pCMP->myCachedTransform);
				}

				tComp->myPosition = transform.GetTranslationV3();
				tComp->myScale = transform.DecomposeScale();
				CU::Matrix3x3f rotation(transform);
				rotation.GetRow(1) = rotation.GetRow(1) / tComp->myScale.x;
				rotation.GetRow(2) = rotation.GetRow(2) / tComp->myScale.y;
				rotation.GetRow(3) = rotation.GetRow(3) / tComp->myScale.z;

				CU::Quaternionf tor = CU::Quaternionf(rotation);
				tComp->myQuaternion = tor;

				tComp->myEuler.ToEulerAngles();

				//std::cout << tor.x<< tor.y << tor.z <<std::endl;

			}
			io.MousePos = oldPos;
			io.MousePosPrev = oldPosPrev;
		}

		return ImGuizmo::IsUsing();

	}
}