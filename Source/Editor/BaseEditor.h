#pragma once
#include "EditorContext.h"
#include "ImGuiInterface.h"
#include "ISTE/Context.h"
#include <vector>
#include <string>
#include "ISTE/Scene/Scene.h"
#include "ISTE/Math/Vec2.h"
#include "Inspector.h"
#include "Gizmos.h"
#include "Systems/MouseObjectSelctionSystem.h"
#include "Tools/AbstractTool.h"
#include "AssetsManager.h"

#include "VisualizationSettings.h"
#include "EditorSettings.h"
#include "IconSettings.h"

#include "Json/json.hpp"


namespace ISTE
{
	bool CompareCStrings(const char* aString, const char* aSecondString, const int anAmountOfLetters);		

	struct SceneLoadInfo
	{
		std::string myScenePath = "";
		std::string mySceneName = "";
	};
	enum class RenderPass
	{
		WorldPosition,
		Albedo,
		PixelNormal,
		VertexNormal,
		Material,
		CustomData,
		EntityData,
		// change above to match GBuffer

		FinalTex,

		Count
	};

	class BaseEditor
	{
	public:
		BaseEditor() = default;
		~BaseEditor();
		void Init();
		void StartFrame(); //Start the frame and do shit.
		void EndFrame(); //Render and end shit.
		void DoWholeImGui(); //Start and end frame.
		void Update();	//temp tjhing becayuse im im -geptge // Good tjhing Goerge, I'm keeping it in. -Loke
		void CheckInputs();
		ID3D11ShaderResourceView* GetSelectedSRV();
		void DrawDisplay();
		void DrawMenuBar();
		void ExitImGui();
		void ObjectMenu(); 
		void SceneMenu();
		void UpdateSceneObjects(); 
		void ImGuizmo();
		void CheckChildren(const std::vector<EntityID>& anEntityListToCheck);
		void DragDrop(EntityID anID = INVALID_ENTITY);
		void DrawGizmos();

		void PrepareIconRenderCommands();
		EntityID CreateEntity();

		void SetTarget(EntityID aID);
		void SnapToFocusedEntity(bool aUseLocal = false);

		inline AssetsManager& GetAssetsManager() { return myAssetsManager; }
		inline ImGuiInterface& GetImGuiInterface() { return myImGuiInterface; }
		inline std::vector<AbstractTool*>& GetTools() { return myTools; }

	private:
		EditorContext myEditorContext;

		ImGuiInterface myImGuiInterface;
		bool myObjectMenuOn = false;
		bool myScenesMenuOn = false;
		bool myPPMenuOn = false;
		bool myFogMenuOn = false;
		bool myGameOnOff = false;
		bool myAssetsManagerOnOff = false;
		bool myPlayerCameraMenuOn = false;
		bool myShaderToolOn = false;
		bool myProfilingMenuOn = false;
		ISTE::Inspector myInspector;
		std::string myObjectSearch = "";
		Gizmos myGizmos;
		//MouseObjectSelectionSystem* myMOSSystem = nullptr;

		ImGuiViewport* myGameViewPort;
		ISTE::AssetsManager myAssetsManager;

		std::vector<AbstractTool*> myTools;
		std::vector<SceneLoadInfo> myBuildScenes;
		std::vector<SceneLoadInfo> myTestScenes;
		std::vector<ISTE::Entity> myEntities;
		std::vector<EntityID> myParents;
		ScreenHelper helper;

		IconSettings myIconSettings;
		VisualizationSettings myVisualizationSettings;
		EditorSettings myEditorSettings;

	public:
		static bool myWantToUpdateSize;

		bool myInspectorDisabled = false;

		//when editor creates a entity store it here
		std::vector<EntityID> myCreatedEntities;
		EntityID myFocusedID = INVALID_ENTITY;
		
		bool myGizmoDraw = false;

		nlohmann::json myCopy;
		bool myValidCopy = false;

		bool myDisplayFocused = false;
		RECT myDisplayRect;
		RenderPass myRenderPass = RenderPass::FinalTex;

		ImDrawData myDrawData;
		ImDrawList* myClonedBackList = nullptr;
		bool myValidBackList = false;
	};
}