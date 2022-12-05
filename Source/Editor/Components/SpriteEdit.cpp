#include "SpriteEdit.h"
#include "imgui/imgui.h"
#include "Commands/CommandFunctions.h"
#include "ComponentFunctions.h"
#include "ISTE/Graphics/ComponentAndSystem/Sprite3DComponent.h"
#include "ISTE/Graphics/ComponentAndSystem/Sprite2DComponent.h"

void ISTE::SpriteEdit::Draw(ISTE::Scene* aScenePointer, Entity anEntity)
{
	Sprite3DComponent(aScenePointer, anEntity);
	Sprite2DComponent(aScenePointer, anEntity);
}

void ISTE::SpriteEdit::Sprite3DComponent(ISTE::Scene* aScenePointer, Entity anEntity)
{
	ISTE::Sprite3DComponent* sprite = aScenePointer->GetComponent<ISTE::Sprite3DComponent>(anEntity.myId);
	if (sprite == nullptr)
	{
		return;
	}
	ImGui::Text("Sprite3D");
	{
		float* color[3] = { &sprite->myColor.r,&sprite->myColor.g ,&sprite->myColor.b };
		ImGui::InputFloat3("Color", *color);
		CF::CheckEditing<CU::Vec3f>(myOriginalColour, &sprite->myColor, myIsEditing);
	}
	{
		float* uv[2] = { &sprite->myUVOffset.x,&sprite->myUVOffset.y };
		ImGui::InputFloat2("UV ##3DSprite", *uv);
		CF::CheckEditing<CU::Vec2f>(myOriginalUV, &sprite->myUVOffset, myIsEditing2);
	}
	{
		float* uvScale[2] = { &sprite->myUVScale.x,&sprite->myUVScale.y };
		ImGui::InputFloat2("UV scale ##3DSprite", *uvScale);
		CF::CheckEditing<CU::Vec2f>(myOriginalUVScale, &sprite->myUVScale, myIsEditing3);
	}
	{
		float* uvRect[4] = { &sprite->myUVStart.x,&sprite->myUVStart.y,&sprite->myUVEnd.x,&sprite->myUVEnd.y };
		ImGui::InputFloat4("UV Rect (Left Top Right Bottom) ##3DSprite", *uvRect);
		CF::CheckEditing<CU::Vec2f>(myOriginalUVStart, &sprite->myUVStart, myIsEditing4);
		CF::CheckEditing<CU::Vec2f>(myOriginalUVEnd, &sprite->myUVEnd, myIsEditing5);
	}
	{
		ImGui::Text("Texture ID:");
		ImGui::SameLine();
		ImGui::Text(std::to_string(sprite->myTextureId).c_str());
	}
	{
		std::string correlatingSamplers[4] = { "Point", "Bilinear", "Trilinear","Anisotropic" };
		ISTE::SamplerState possibleSamplers[4] = { ISTE::SamplerState::ePoint,ISTE::SamplerState::eBiliniear,ISTE::SamplerState::eTriLinear,ISTE::SamplerState::eAnisotropic };
		EF::SetList<ISTE::SamplerState>((int)sprite->mySamplerState, sprite->mySamplerState, (std::string)"Sampler State", correlatingSamplers, possibleSamplers);
	}
	{
		std::string correlatingAdressModes[3] = { "Clamp", "Mirror", "Wrap" };
		ISTE::AdressMode possibleAdressModes[3] = { ISTE::AdressMode::eClamp,ISTE::AdressMode::eMirror,ISTE::AdressMode::eWrap };
		EF::SetList<ISTE::AdressMode>((int)sprite->myAdressMode, sprite->myAdressMode, (std::string)"Adress Mode", correlatingAdressModes, possibleAdressModes);
	}
	ImGui::Separator();
}

void ISTE::SpriteEdit::Sprite2DComponent(ISTE::Scene* aScenePointer, Entity anEntity)
{
	ISTE::Sprite2DComponent* sprite = aScenePointer->GetComponent<ISTE::Sprite2DComponent>(anEntity.myId);
	if (sprite == nullptr)
	{
		return;
	}
	ImGui::Text("Sprite2D");
	{
		float* color[3] = { &sprite->myColor.r,&sprite->myColor.g ,&sprite->myColor.b };
		ImGui::InputFloat3("Color ##2DSprite", *color);
		CF::CheckEditing<CU::Vec3f>(myOriginalColour, &sprite->myColor, myIsEditing);
	}
	{
		float* uv[2] = { &sprite->myUVOffset.x,&sprite->myUVOffset.y };
		ImGui::InputFloat2("UV ##2DSprite", *uv);
		CF::CheckEditing<CU::Vec2f>(myOriginalUV, &sprite->myUVOffset, myIsEditing2);
	}
	{
		float* uvScale[2] = { &sprite->myUVScale.x,&sprite->myUVScale.y };
		ImGui::InputFloat2("UV scale ##2DSprite", *uvScale);
		CF::CheckEditing<CU::Vec2f>(myOriginalUVScale, &sprite->myUVScale, myIsEditing3);
	}
	{
		float* uvRect[4] = { &sprite->myUVStart.x,&sprite->myUVStart.y,&sprite->myUVEnd.x,&sprite->myUVEnd.y };
		ImGui::InputFloat4("UV Rect (Left Top Right Bottom) ##2DSprite", *uvRect);
		CF::CheckEditing<CU::Vec2f>(myOriginalUVStart, &sprite->myUVStart, myIsEditing4);
		CF::CheckEditing<CU::Vec2f>(myOriginalUVEnd, &sprite->myUVEnd, myIsEditing5);
	}
	{
		ImGui::Text("Texture ID:");
		ImGui::SameLine();
		ImGui::Text(std::to_string(sprite->myTextureId).c_str());
	}
	{
		std::string correlatingSamplers[4] = { "Point", "Bilinear", "Trilinear","Anisotropic" };
		ISTE::SamplerState possibleSamplers[4] = { ISTE::SamplerState::ePoint,ISTE::SamplerState::eBiliniear,ISTE::SamplerState::eTriLinear,ISTE::SamplerState::eAnisotropic };
		EF::SetList<ISTE::SamplerState>((int)sprite->mySamplerState, sprite->mySamplerState, (std::string)"Sampler State", correlatingSamplers, possibleSamplers);
	}
	{
		std::string correlatingAdressModes[3] = { "Clamp", "Mirror", "Wrap" };
		ISTE::AdressMode possibleAdressModes[3] = { ISTE::AdressMode::eClamp,ISTE::AdressMode::eMirror,ISTE::AdressMode::eWrap };
		EF::SetList<ISTE::AdressMode>((int)sprite->myAdressMode, sprite->myAdressMode, (std::string)"Adress Mode", correlatingAdressModes, possibleAdressModes);
	}
	ImGui::Separator();

}