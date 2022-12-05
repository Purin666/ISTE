#include "VFXToolFunctions.h"

#include "imgui/imgui.h"
#include "imgui/misc/cpp/imgui_stdlib.h"

#include "ISTE/Context.h"
#include "ISTE/Graphics/Resources/TextureManager.h"

#include "ISTE/VFX/SpriteParticles/Sprite3DParticleHandler.h"
#include "ISTE/VFX/SpriteParticles/Sprite3DParticleDefines.h"
#include "ISTE/VFX/SpriteParticles/Sprite2DParticleHandler.h"
#include "ISTE/VFX/SpriteParticles/Sprite2DParticleDefines.h"

#include "ISTE/VFX/ModelVFX/ModelVFXHandler.h"
#include "ISTE/VFX/ModelVFX/ModelVFXDefines.h"

#include "EditorContext.h"
#include "AssetsManager.h"

void ISTE::DrawModelVFXEditor(ModelVFXData& data)
{
	std::string title;

	//ImGui::Spacing();
	// Model and Textures
	if (ImGui::CollapsingHeader("Resources"))
	{
		ImGui::Text("Model: ");
		ImGui::SameLine();
		ImGui::Text(data.myModelPath.data());
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(".fbx", 0))
			{
				data.myModelPath = *(std::string*)payload->Data;
				Context::Get()->myModelVFXHandler->LoadModelsAndTextures();
			}
		}
		if (ImGui::GetIO().MouseDoubleClicked[0] && ImGui::IsItemClicked())
		{
			EditorContext::Get()->myAssetsManager->LookUp(data.myModelPath);
		}
		if (ImGui::Button("Add Texture"))
		{
			if (data.myTexturePath.size() < 3)
			{
				data.myTexturePath.push_back("Default");
				data.mySample.push_back(CU::Vec3f());
				data.myUVOverLife.push_back(CU::Vec2f());
			}
		}
		for (size_t i = 0; i < data.myTexturePath.size(); i++)
		{
			ImGui::PushID(&data.myTexturePath[i]);
			ImGui::Separator();
			ImGui::Text("Texture: ");
			ImGui::SameLine();
			ImGui::Text(data.myTexturePath[i].data());
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(".dds", 0))
				{
					data.myTexturePath[i] = *(std::string*)payload->Data;
					Context::Get()->myModelVFXHandler->LoadModelsAndTextures();
				}
			}
			if (ImGui::GetIO().MouseDoubleClicked[0] && ImGui::IsItemClicked())
			{
				EditorContext::Get()->myAssetsManager->LookUp(data.myTexturePath[i]);
			}
			if (data.myTexturePath.size() > 1)
			{
				ImGui::SameLine();
				if (ImGui::Button("Remove"))
				{
					data.myTexturePath.erase(data.myTexturePath.begin() + i);
					data.mySample.erase(data.mySample.begin() + i);
					data.myUVOverLife.erase(data.myUVOverLife.begin() + i);
					ImGui::PopID();//&data.myTexturePath[k]
					break;
				}
			}
			ImGui::Text("Sample rgb");
			ImGui::DragFloat3("##sample", (float*)&data.mySample[i]);
			ImGui::Text("UV Change Over Life");
			ImGui::DragFloat2("##uv", (float*)&data.myUVOverLife[i], 0.001f);
			ImGui::PopID();//&data.myTexturePath[k]
		}
	}

	//Generic
	if (ImGui::CollapsingHeader("Generic"))
	{
		ImGui::DragFloat2("Lifetime", (float*)&data.myLifetime, 0.001f, 0.f);
		//ImGui::DragFloat2("Delay", (float*)&data.myDelay, 0.01f, 0.f);
		//ImGui::DragFloat2("FirstDelay", (float*)&data.myFirstDelay, 0.01f, 0.f);
		ImGui::Checkbox("Loop", &data.myShouldLoop);
	}
	// Color
	if (ImGui::CollapsingHeader("Color"))
	{
		ImGui::ColorEdit3("Initial Color", (float*)&data.myInitialColor);
		if (ImGui::Button("Add Color Over Life"))
			data.myColorOverLife.push_back(CU::Vec3f());
		for (size_t i = 0; i < data.myColorOverLife.size(); i++)
		{
			title = "Color " + std::to_string(i);
			ImGui::ColorEdit3(title.data(), (float*)&data.myColorOverLife[i]);

			ImGui::SameLine();
			ImGui::PushID(&data.myColorOverLife[i]);
			title = "Remove";
			if (ImGui::Button(title.data()))
				data.myColorOverLife.erase(data.myColorOverLife.begin() + i);
			ImGui::PopID();//&data.myColorOverLife[i]
		}
	}

	// Size
	if (ImGui::CollapsingHeader("Size"))
	{
		ImGui::DragFloat3("Initial Size Min", (float*)&data.myInitialSize.myMin, 0.001f);
		ImGui::DragFloat3("Initial Size Max", (float*)&data.myInitialSize.myMax, 0.001f);
		if (ImGui::Button("Add Size Over Life"))
			data.mySizeOverLife.push_back(CU::Vec3f());
		for (size_t i = 0; i < data.mySizeOverLife.size(); i++)
		{
			title = "Scale " + std::to_string(i);
			ImGui::DragFloat3(title.data(), (float*)&data.mySizeOverLife[i], 0.001f, 0.f);

			ImGui::SameLine();
			ImGui::PushID(&data.mySizeOverLife[i]);
			title = "Remove";
			if (ImGui::Button(title.data()))
				data.mySizeOverLife.erase(data.mySizeOverLife.begin() + i);
			ImGui::PopID();//&data.mySizeOverLife[i]
		}
	}
	// Rotation
	if (ImGui::CollapsingHeader("Rotation"))
	{
		ImGui::DragFloat3("Initial Rotation Min", (float*)&data.myInitialRotation.myMin, 0.001f);
		ImGui::DragFloat3("Initial Rotation Max", (float*)&data.myInitialRotation.myMax, 0.001f);
		if (ImGui::Button("Add Rotation Rate Over Life"))
			data.myRotationRateOverLife.push_back(CU::Vec3f());
		for (size_t i = 0; i < data.myRotationRateOverLife.size(); i++)
		{
			title = "Rotation " + std::to_string(i);
			ImGui::DragFloat3(title.data(), (float*)&data.myRotationRateOverLife[i], 0.001f);

			ImGui::SameLine();
			ImGui::PushID(&data.myRotationRateOverLife[i]);
			title = "Remove";
			if (ImGui::Button(title.data()))
				data.myRotationRateOverLife.erase(data.myRotationRateOverLife.begin() + i);
			ImGui::PopID();//&data.myRotationRateOverLife[i]
		}
	}
	// Spawn Location
	if (ImGui::CollapsingHeader("Location"))
	{
		ImGui::DragFloat3("Initial Position Box Min", (float*)&data.myInitialBoxPosition.myMin, 0.001f);
		ImGui::DragFloat3("Initial Position Box Max", (float*)&data.myInitialBoxPosition.myMax, 0.001f);
		ImGui::DragFloat2("Initial Position Sphere", (float*)&data.myInitialSpherePosition, 0.001f);
	}
	// Physics
	if (ImGui::CollapsingHeader("Physics"))
	{
		ImGui::DragFloat3("Initial Velocity Min", (float*)&data.myInitialVelocity.myMin, 0.001f);
		ImGui::DragFloat3("Initial Velocity Max", (float*)&data.myInitialVelocity.myMax, 0.001f);
		ImGui::DragFloat2("Initial Mass", (float*)&data.myMass, 0.001f);
		ImGui::DragFloat2("Initial Resistance", (float*)&data.myResistance, 0.001f);
	}
}

void ISTE::DrawEmitter3DEditor(Emitter3DData& data)
{
	std::string title;

	ImGui::Spacing();
	if (ImGui::CollapsingHeader("Texture"))
	{
		ImGui::Text("Texture: ");
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(".dds", 0))
			{
				data.myTexturePath = *(std::string*)payload->Data;
				data.myTextureID = Context::Get()->myTextureManager->LoadTexture(std::wstring(data.myTexturePath.begin(), data.myTexturePath.end()));
			}
		}
		if (ImGui::GetIO().MouseDoubleClicked[0] && ImGui::IsItemClicked())
		{
			EditorContext::Get()->myAssetsManager->LookUp(data.myTexturePath);
		}
		ImGui::SameLine();
		ImGui::Text(data.myTexturePath.data());

		ImGui::DragInt2("Sub Images H/V", (int*)&data.mySubImages, 0.1f, 1, 16);
		ImGui::DragFloat("Sub Images / sec", (float*)&data.mySubImagesPerSecond, 0.001f);
		ImGui::Checkbox("Random Sub Image", &data.myPickRandomSubImage);
	}
	if (ImGui::CollapsingHeader("Generic"))
	{
		ImGui::DragFloat2("Emitter Duration", (float*)&data.myEmitterDuration, 0.001f, 0.f);
		ImGui::DragFloat2("Particle Lifetime", (float*)&data.myLifeTime, 0.001f, 0.f);
		ImGui::DragFloat2("Initial Delay", (float*)&data.myInitialDelay, 0.001f, 0.f);
		ImGui::DragFloat2("Spawn Rate", (float*)&data.mySpawnRate, 0.001f, 0.f);
		ImGui::DragInt2("Initial Spawn Amount ", (int*)&data.myInitialSpawnAmount, 0.001f, 0);
		ImGui::DragInt2("Spawn Amount ", (int*)&data.mySpawnAmount, 0.001f, 0);
		ImGui::Checkbox("Use Local Space", &data.myUseLocalSpace);

		ImGui::Checkbox("Face Towards Camera", &data.myFaceTowardsCamera);
		if (!data.myFaceTowardsCamera)
		{
			ImGui::DragFloat3("Face Direction", (float*)&data.myFaceDirection, 0.001f);
			ImGui::Spacing();
		}
	}
	if (ImGui::CollapsingHeader("Color"))
	{
		ImGui::ColorEdit3("Initial Color", (float*)&data.myInitialColor);
		ImGui::DragFloat("Initial Alpha", &data.myInitialAlpha);
		if (ImGui::Button("Add Color Over Life"))
			data.myColorOverLife.push_back(data.myInitialColor);
		if (ImGui::Button("Add Alpha Over Life"))
			data.myAlphaOverLife.push_back(data.myInitialAlpha);
		for (size_t i = 0; i < data.myColorOverLife.size(); i++)
		{
			title = "Color " + std::to_string(i);
			ImGui::ColorEdit3(title.data(), (float*)&data.myColorOverLife[i]);

			ImGui::SameLine();
			ImGui::PushID(&data.myColorOverLife[i]);
			title = "Remove";
			if (ImGui::Button(title.data()))
				data.myColorOverLife.erase(data.myColorOverLife.begin() + i);
			ImGui::PopID();
		}
		for (size_t i = 0; i < data.myAlphaOverLife.size(); i++)
		{
			title = "Alpha " + std::to_string(i);
			ImGui::DragFloat(title.data(), &data.myAlphaOverLife[i]);

			ImGui::SameLine();
			ImGui::PushID(&data.myAlphaOverLife[i]);
			title = "Remove";
			if (ImGui::Button(title.data()))
				data.myAlphaOverLife.erase(data.myAlphaOverLife.begin() + i);
			ImGui::PopID();
		}
	}
	if (ImGui::CollapsingHeader("Size"))
	{
		ImGui::DragFloat3("Initial Size Min", (float*)&data.myInitialSize.myMin, 0.001f, 0);
		ImGui::DragFloat3("Initial Size Max", (float*)&data.myInitialSize.myMax, 0.001f, 0);
		if (ImGui::Button("Add Size Over Life"))
			data.mySizeOverLife.push_back(data.myInitialSize.myMax);
		for (size_t i = 0; i < data.mySizeOverLife.size(); i++)
		{
			title = "Size " + std::to_string(i);
			ImGui::DragFloat3(title.data(), (float*)&data.mySizeOverLife[i], 0.001f, 0.f);

			ImGui::SameLine();
			ImGui::PushID(&data.mySizeOverLife[i]);
			title = "Remove";
			if (ImGui::Button(title.data()))
				data.mySizeOverLife.erase(data.mySizeOverLife.begin() + i);
			ImGui::PopID();
		}
	}
	if (ImGui::CollapsingHeader("Rotation"))
	{
		ImGui::DragFloat2("Initial Rotation", (float*)&data.myInitialRotation, 0.001f);
		ImGui::DragFloat2("Rotation Rate", (float*)&data.myRotationRate, 0.001f);
	}
	if (ImGui::CollapsingHeader("Position"))
	{
		ImGui::DragFloat3("Spawn Box Min", (float*)&data.myInitialBoxPosition.myMin, 0.001f);
		ImGui::DragFloat3("Spawn Box Max", (float*)&data.myInitialBoxPosition.myMax, 0.001f);
		ImGui::DragFloat2("Spawn Radius", (float*)&data.myInitialSpherePosition, 0.001f);
		ImGui::Checkbox("Clamp Within Box", &data.myClampWithinBox);
	}
	if (ImGui::CollapsingHeader("Physics"))
	{
		ImGui::DragFloat3("Initial Velocity Min", (float*)&data.myInitialVelocity.myMin, 0.001f);
		ImGui::DragFloat3("Initial Velocity Max", (float*)&data.myInitialVelocity.myMax, 0.001f);
		ImGui::DragFloat3("Const Acceleration Min", (float*)&data.myConstAcceleration.myMin, 0.001f);
		ImGui::DragFloat3("Const Acceleration Max", (float*)&data.myConstAcceleration.myMax, 0.001f);
		ImGui::DragFloat2("Initial Mass", (float*)&data.myMass, 0.001f);
		ImGui::DragFloat2("Initial Resistance", (float*)&data.myResistance, 0.001f);
	}
}

void ISTE::DrawEmitter2DEditor(Emitter2DData& data)
{
	std::string title;

	ImGui::Spacing();
	if (ImGui::CollapsingHeader("Texture"))
	{
		ImGui::Text("Texture: ");
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(".dds", 0))
			{
				data.myTexturePath = *(std::string*)payload->Data;
				data.myTextureID = Context::Get()->myTextureManager->LoadTexture(std::wstring(data.myTexturePath.begin(), data.myTexturePath.end()));
			}
		}
		if (ImGui::GetIO().MouseDoubleClicked[0] && ImGui::IsItemClicked())
		{
			EditorContext::Get()->myAssetsManager->LookUp(data.myTexturePath);
		}
		ImGui::SameLine();
		ImGui::Text(data.myTexturePath.data());

		ImGui::DragInt2("Sub Images H/V", (int*)&data.mySubImages, 0.1f, 1, 16);
		ImGui::Checkbox("Random Sub Image", &data.myPickRandomSubImage);
		ImGui::DragFloat("Sub Images / sec", (float*)&data.mySubImagesPerSecond, 0.001f);
	}
	if (ImGui::CollapsingHeader("Generic"))
	{
		ImGui::DragFloat2("Emitter Duration", (float*)&data.myEmitterDuration, 0.001f, 0.f);
		ImGui::DragFloat2("Particle Lifetime", (float*)&data.myLifeTime, 0.001f, 0.f);
		ImGui::DragFloat2("Initial Delay", (float*)&data.myInitialDelay, 0.001f, 0.f);
		ImGui::DragFloat2("Spawn Rate", (float*)&data.mySpawnRate, 0.001f, 0.f);
		ImGui::DragInt2("Initial Spawn Amount", (int*)&data.myInitialSpawnAmount, 0.001f, 0);
		ImGui::DragInt2("Spawn Amount", (int*)&data.mySpawnAmount, 0.001f, 0);
		ImGui::Checkbox("Use Local Space", &data.myUseLocalSpace);
	}
	if (ImGui::CollapsingHeader("Color"))
	{
		ImGui::ColorEdit3("Initial Color", (float*)&data.myInitialColor);
		ImGui::DragFloat("Initial Alpha", &data.myInitialAlpha);
		if (ImGui::Button("Add Color Over Life"))
			data.myColorOverLife.push_back(data.myInitialColor);
		if (ImGui::Button("Add Alpha Over Life"))
			data.myAlphaOverLife.push_back(data.myInitialAlpha);
		for (size_t i = 0; i < data.myColorOverLife.size(); i++)
		{
			title = "Color " + std::to_string(i);
			ImGui::ColorEdit3(title.data(), (float*)&data.myColorOverLife[i]);

			ImGui::SameLine();
			ImGui::PushID(&data.myColorOverLife[i]);
			title = "Remove";
			if (ImGui::Button(title.data()))
				data.myColorOverLife.erase(data.myColorOverLife.begin() + i);
			ImGui::PopID();
		}
		for (size_t i = 0; i < data.myAlphaOverLife.size(); i++)
		{
			title = "Alpha " + std::to_string(i);
			ImGui::DragFloat(title.data(), &data.myAlphaOverLife[i]);

			ImGui::SameLine();
			ImGui::PushID(&data.myAlphaOverLife[i]);
			title = "Remove";
			if (ImGui::Button(title.data()))
				data.myAlphaOverLife.erase(data.myAlphaOverLife.begin() + i);
			ImGui::PopID();
		}
	}
	if (ImGui::CollapsingHeader("Size"))
	{
		ImGui::DragFloat2("Initial Size Min", (float*)&data.myInitialSize.myMin, 0.001f, 0);
		ImGui::DragFloat2("Initial Size Max", (float*)&data.myInitialSize.myMax, 0.001f, 0);
		if (ImGui::Button("Add Size Over Life"))
			data.mySizeOverLife.push_back(data.myInitialSize.myMax);
		for (size_t i = 0; i < data.mySizeOverLife.size(); i++)
		{
			title = "Size " + std::to_string(i);
			ImGui::DragFloat2(title.data(), (float*)&data.mySizeOverLife[i], 0.001f, 0.f);

			ImGui::SameLine();
			ImGui::PushID(&data.mySizeOverLife[i]);
			title = "Remove";
			if (ImGui::Button(title.data()))
				data.mySizeOverLife.erase(data.mySizeOverLife.begin() + i);
			ImGui::PopID();
		}
	}
	if (ImGui::CollapsingHeader("Rotation"))
	{
		ImGui::DragFloat2("Initial Rotation", (float*)&data.myInitialRotation, 0.001f);
		ImGui::DragFloat2("Rotation Rate", (float*)&data.myRotationRate, 0.001f);
	}
	if (ImGui::CollapsingHeader("Position"))
	{
		ImGui::DragFloat2("Spawn Box Min", (float*)&data.myInitialBoxPosition.myMin, 0.001f);
		ImGui::DragFloat2("Spawn Box Max", (float*)&data.myInitialBoxPosition.myMax, 0.001f);
		ImGui::DragFloat2("Spawn Radius", (float*)&data.myInitialCirclePosition, 0.001f);
		ImGui::Checkbox("Clamp Within Box", &data.myClampWithinBox);
	}
	if (ImGui::CollapsingHeader("Physics"))
	{
		ImGui::DragFloat2("Initial Velocity Min", (float*)&data.myInitialVelocity.myMin, 0.001f);
		ImGui::DragFloat2("Initial Velocity Max", (float*)&data.myInitialVelocity.myMax, 0.001f);
		ImGui::DragFloat2("Const Acceleration Min", (float*)&data.myConstAcceleration.myMin, 0.001f);
		ImGui::DragFloat2("Const Acceleration Max", (float*)&data.myConstAcceleration.myMax, 0.001f);
		ImGui::DragFloat2("Initial Mass", (float*)&data.myMass, 0.001f);
		ImGui::DragFloat2("Initial Resistance", (float*)&data.myResistance, 0.001f);
	}
}
