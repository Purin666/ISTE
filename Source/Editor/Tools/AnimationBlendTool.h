#pragma once
#include "AbstractTool.h"
#include "imguizmo/GraphEditor.h"
#include "ISTE/Math/Vec2.h"
#include "ISTE/Graphics/Resources/AnimationDefines.h"
#include "ISTE/Graphics/Resources/Model.h"

#include <vector>
namespace ISTE
{
	 
	struct AnimBlendDelegate : public GraphEditor::Delegate 
	{
	public:
		bool AllowedLink(GraphEditor::NodeIndex from, GraphEditor::NodeIndex to) override;
		void SelectNode(GraphEditor::NodeIndex nodeIndex, bool selected) override;
		void MoveSelectedNodes(const ImVec2 delta) override;
		void RightClick(GraphEditor::NodeIndex nodeIndex, GraphEditor::SlotIndex slotIndexInput, GraphEditor::SlotIndex slotIndexOutput) override;
		void AddLink(GraphEditor::NodeIndex inputNodeIndex, GraphEditor::SlotIndex inputSlotIndex, GraphEditor::NodeIndex outputNodeIndex, GraphEditor::SlotIndex outputSlotIndex) override;
		void DelLink(GraphEditor::LinkIndex linkIndex) override;
		void CustomDraw(ImDrawList* drawList, ImRect rectangle, GraphEditor::NodeIndex nodeIndex) override;

		const size_t		GetTemplateCount() override
		{
			return sizeof(myTemplates) / sizeof(GraphEditor::Template);
		}
		const size_t		GetNodeCount() override
		{
			return myNodes.size();
		}
		const size_t		GetLinkCount() override
		{
			return myLinks.size();
		}
		ModelID&			GetModel() { return myModelID; }
		AnimationBlendID&	GetBlendData() { return myBlendDataID; }

		const GraphEditor::Template GetTemplate(GraphEditor::TemplateIndex index) override
		{
			return myTemplates[index];
		}
		const GraphEditor::Node		GetNode(GraphEditor::NodeIndex index) override
		{
			const auto& myNode = myNodes[index];
			return GraphEditor::Node
			{
				myNode.myName.c_str(),
				myNode.myTemplateIndex,
				ImRect(ImVec2(myNode.myPos.x, myNode.myPos.y), ImVec2(myNode.myPos.x + 200, myNode.myPos.y + 200)),
				myNode.mySelectedFlag
			};
		}
		const GraphEditor::Link		GetLink(GraphEditor::LinkIndex index) override
		{
			return myLinks[index];
		} 

		static const inline GraphEditor::Template myTemplates[]{
			{
				IM_COL32(160, 0, 0, 255),
				IM_COL32(100, 0, 0, 255),
				IM_COL32(110, 0, 0, 255),
				2,
				nullptr,
				nullptr,
				1,
				nullptr,
				nullptr
			},
			{
				IM_COL32(0, 160, 0, 255),
				IM_COL32(0, 100, 0, 255),
				IM_COL32(0, 110, 0, 255),
				0,
				nullptr,
				nullptr,
				1,
				nullptr,
				nullptr
			},
			{
				IM_COL32(0, 0, 160, 255),
				IM_COL32(0, 0, 100, 255),
				IM_COL32(0, 0, 110, 255),
				1,
				nullptr,
				nullptr,
				0,
				nullptr,
				nullptr
			}
		};

	private:
		struct AnimBlendNode{
			std::string					myName;
			CU::Vec2f					myPos;
			GraphEditor::TemplateIndex	myTemplateIndex;
			AnimationBlendNode			myBlendNode;
			bool						mySelectedFlag;
		};
		AnimBlendNode*					myCurrentSelectedNode;
		std::vector<AnimBlendNode>		myNodes;
		std::vector<GraphEditor::Link>	myLinks;
		AnimationBlendID				myBlendDataID = AnimationBlendID(-1);
		ModelID							myModelID = ModelID(-1);

	};

	struct Context;
	class AnimationBlendTool : public AbstractTool
	{
	public:
		void Init(BaseEditor*) override;
		void Draw();

	private:
		void NodeGraph();
		void RenderPreview();
		void NodeInspector();

		GraphEditor::ViewState	viewState;
		GraphEditor::Options	myGEOptions;
		std::string				myBlendDataPreview;
		AnimBlendDelegate		myAnimBlendDelegate;
		Context*				myCtx;


	};

};
