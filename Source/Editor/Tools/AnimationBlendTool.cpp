#include "AnimationBlendTool.h"
#include "ISTE/Context.h"

#include "imgui/imgui.h"
#include "ISTE/Graphics/Resources/AnimationManager.h"




void ISTE::AnimationBlendTool::Init(BaseEditor*)
{
	myToolName = "Animation Blend Tool";
	myCtx = Context::Get();
}

void ISTE::AnimationBlendTool::Draw()
{
	if (!myActive)
		return;
	ImGui::Begin("Animation Blend Tool");
	NodeInspector();
	NodeGraph();
	RenderPreview();
	ImGui::End();
}

void ISTE::AnimationBlendTool::NodeGraph()
{
	if (myAnimBlendDelegate.GetModel() == ModelID(-1) || myAnimBlendDelegate.GetBlendData() == AnimationBlendID(-1))
		return;

	GraphEditor::FitOnScreen fit = GraphEditor::Fit_None;
	GraphEditor::Show(myAnimBlendDelegate, myGEOptions, viewState, true, &fit);
}

void ISTE::AnimationBlendTool::RenderPreview()
{
	ImGui::Begin("Animation Preview");
	ImGui::Image(ImTextureID(),ImGui::GetWindowSize()); 
	ImGui::End();
}

void ISTE::AnimationBlendTool::NodeInspector()
{
	ImGui::Begin("Animaiton Blend Inspector");
	if (ImGui::CollapsingHeader("Animation Blend Data"))
	{


		if (ImGui::Button("Save Animaiton Blend"))
		{
			//Save blend
		}
		 
		//auto blendDataBucket = myCtx->myAnimationManager->GetAnimationBlendList().GetBucket();
		//size_t blendCount = myCtx->myAnimationManager->GetAnimationBlendList().GetSize();

		static int currentBlend = 0; 

		std::string selectableString;

		//if (ImGui::BeginCombo("Animation Blends", myBlendDataPreview.data()))
		//{
		//	for (size_t i = 0; i < blendCount; i++)
		//	{
		//		selectableString = blendDataBucket[i].myValue.myName + "##" + std::to_string(i);
		//		if(ImGui::Selectable(selectableString.data(), currentBlend == i))
		//		{
		//			currentBlend = i;
		//			myAnimBlendDelegate.GetBlendData() = blendDataBucket[i].myValue.myID;
		//		}
		//	}
		//	ImGui::End();
		//}



	}


	ImGui::End();


	myAnimBlendDelegate.GetModel();
}

bool ISTE::AnimBlendDelegate::AllowedLink(GraphEditor::NodeIndex from, GraphEditor::NodeIndex to)
{
	return true;
}

void ISTE::AnimBlendDelegate::SelectNode(GraphEditor::NodeIndex nodeIndex, bool selected)
{
	myCurrentSelectedNode					= &myNodes[nodeIndex];
	myCurrentSelectedNode->mySelectedFlag	= selected;
}

void ISTE::AnimBlendDelegate::MoveSelectedNodes(const ImVec2 delta)
{ 
	myCurrentSelectedNode->myPos.x += delta.x;
	myCurrentSelectedNode->myPos.y += delta.y;
}

void ISTE::AnimBlendDelegate::RightClick(GraphEditor::NodeIndex nodeIndex, GraphEditor::SlotIndex slotIndexInput, GraphEditor::SlotIndex slotIndexOutput)
{
}

void ISTE::AnimBlendDelegate::AddLink(GraphEditor::NodeIndex inputNodeIndex, GraphEditor::SlotIndex inputSlotIndex, GraphEditor::NodeIndex outputNodeIndex, GraphEditor::SlotIndex outputSlotIndex)
{
	myLinks.push_back({ inputNodeIndex, inputSlotIndex, outputNodeIndex, outputSlotIndex });
	//gets node types and links them up in AD
}

void ISTE::AnimBlendDelegate::DelLink(GraphEditor::LinkIndex linkIndex)
{
	myLinks.erase(myLinks.begin() + linkIndex);
	//gets node types and unlinks them in AD
}

void ISTE::AnimBlendDelegate::CustomDraw(ImDrawList* drawList, ImRect rectangle, GraphEditor::NodeIndex nodeIndex)
{
	drawList->AddLine(rectangle.Min, rectangle.Max, IM_COL32(0, 0, 0, 255));
	drawList->AddText(rectangle.Min, IM_COL32(255, 128, 64, 255), "Draw");
}
