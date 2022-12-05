#include "CustomShaderEdit.h"
#include "ISTE/Graphics/ComponentAndSystem/CustomShaderComponent.h"
#include "imgui/imgui.h"
//#include "Commands/CommandManager.h"
//#include "Commands/VariableCommand.h"
#include "ComponentFunctions.h"

void ISTE::CustomShaderEdit::Draw(ISTE::Scene* aScenePointer, Entity anEntity)
{
	ISTE::CustomShaderComponent* shaderComponent = aScenePointer->GetComponent<ISTE::CustomShaderComponent>(anEntity.myId);
	if (shaderComponent == nullptr) // Make sure it exists.
	{
		return;
	}
	if (!ImGui::TreeNode("CustomShaderComponent")) // Make sure you want to know about it.
	{
		return;
	}
	std::string correlatingStrings[8] = { "Default Model Shader", "Color Shader", "Sprite Shader", "Animated Normal Shader", "Player Damaged", "Enemy Damaged", "Magic Armor", "Area Of Effect At Player" };
	Shaders availableShaders[8] = { Shaders::eDefaultModelShader , Shaders::eColorShader, Shaders::eSpriteShader, Shaders::eAnimatedNormalShader, Shaders::ePlayerDamaged, Shaders::eEnemyDamaged,Shaders::eMagicArmor , Shaders::eAreaOfEffectAtPlayer };
	EF::SetList<Shaders>((int)shaderComponent->myShader, shaderComponent->myShader, (std::string)"Shader Type", correlatingStrings, availableShaders);

	ImGui::TreePop();
	ImGui::Separator();
}
