#pragma once

#include "UIDefines.h"
#include <unordered_map>
#include <map>
#include <queue>

#include "ISTE/Graphics/ComponentAndSystem/Sprite2DCommand.h"

namespace ISTE
{
	struct Context;

	class UIHandler
	{
	public:
		~UIHandler();
		void Init();

		void UpdateMouse();
		void EnableMouseEmitter(const bool aFlag);
		void RenderMouse();

		inline void AddCallbackToExecute(std::function<void()> aCallback) { myCallbacksToExecute.push(aCallback); }

		void LoadTexturesAndFonts();
		void LoadCallbacks();


		std::unordered_map<std::string, UI> myUIDatas;

		std::map<std::string, std::function<void()>> myCallbacks;
		std::map<GameplayUIType, std::string> myGameplayTypes;

		std::queue<std::function<void()>> myCallbacksToExecute;

	private:
		void InitGameplayTypes();
		void InitCallbacks();
		void InitMouse();

		Context* myCtx;

		Sprite2DRenderCommand myMouse;
		int myClickEmitter = -1;
		int myMouseEmitter = -1;
		bool myMouseEmitterFlag = true;
	};
}