#include "TextTrigger.h"

#include "ISTE/Context.h"
#include "ISTE/Graphics/GraphicsEngine.h"
#include "ISTE/Graphics/ComponentAndSystem/TransformComponent.h"

#include "ISTE/Scene/SceneHandler.h"
#include "ISTE/Scene/Scene.h"
#include "ISTE/ECSB/PlayerBehaviour.h"

#include "ISTE/Time/TimeHandler.h"
#include "ISTE/Time/TimerDefines.h"

#include "ISTE/Graphics/Resources/TextureManager.h"
#include "ISTE/Graphics/ComponentAndSystem/SpriteDrawerSystem.h"
#include "ISTE/Graphics/ComponentAndSystem/Sprite2DComponent.h"
#include "ISTE/Text/TextComponent.h"

#include "ISTE/CU/MemTrack.hpp"

void ISTE::TextTrigger::Init()
{
	myScene = &ISTE::Context::Get()->mySceneHandler->GetActiveScene();

	myOffset = { -0.25f, -0.64f, 0.f };

	myIsInactive = false;
	myHasUpdated = false;
	myUnderFollowTimer = false;
	myCanUseNextIndex = true;
	myCanFlipPage = true;
	myRenderBox = false;

	myTempText = "";

	myStringIndex = 0;
	myPageIndex = -1;

	myBox.myPosition = { 0.02f, -0.6f };
	myBox.myScale = { 0.7f, 0.4f };
	myBox.myTextureId[ALBEDO_MAP] = Context::Get()->myTextureManager->LoadTexture(L"../Assets/Sprites/Ui/Ui HUD/UI_dialogueBox.dds").myValue;

	const CU::Vec2Ui res = Context::Get()->myWindow->GetResolution();
	const float ratio = (float)res.x / res.y;

	myTextId = myScene->NewEntity();

	TransformComponent* transform = myScene->AssignComponent<TransformComponent>(myTextId);
	transform->myPosition = CU::Vec3f(-10000.f, -10000.f, 0.f); // BEGONE
	//transform->myEuler; // TODO:: Use anElement.myTransform.myRotation
	transform->myScale = CU::Vec3f(1.f, 1.f, 0.f);
	transform->myScale.y *= ratio;

	TextComponent* text = myScene->AssignComponent<TextComponent>(myTextId);
	text->myFontID = Context::Get()->myTextureManager->LoadFont(myFont, myFontSize);
	text->myText = myText;
	text->myPivot = { 0.f, 0.f };
	text->myScale = 1.f;
	text->myColor = { 1.f, 1.f, 1.f, 1.f };
}

void ISTE::TextTrigger::Update(float aDeltaTime)
{
	if (myIsInactive)
		return;

	if (myRenderBox)
		Context::Get()->mySystemManager->GetSystem<SpriteDrawerSystem>()->Add2DSpriteRenderCommand(myBox);

	TransformComponent* textTransform = myScene->GetComponent<TransformComponent>(myTextId);

	if (myUnderFollowTimer)
	{
		if (myOneUse && myHasUpdated)
		{
			textTransform->myPosition = CU::Vec3f(-10000.f, -10000.f, 0.f); // BEGONE
			myIsInactive = true;
			myRenderBox = false;
			return;
		}

		myRenderBox = true;

		if (myUseAnimation)
		{
			if (((myUsePages) ? ((myPageIndex == -1) ? myText.empty() : myPages[myPageIndex].empty()) : myText.empty())
				|| ((myUsePages) ? ((myPageIndex == -1) ? myText.size() : myPages[myPageIndex].size()) : myText.size()) <= 1)
				return;

			if (myStringIndex >= ((myUsePages) ? ((myPageIndex == -1) ? myText.size() : myPages[myPageIndex].size()) : myText.size())
				|| !myCanUseNextIndex)
			{
				if (myStringIndex >= ((myUsePages) ? ((myPageIndex == -1) ? myText.size() : myPages[myPageIndex].size()) : myText.size())
					&& myOneUse && (myUsePages ? (myPageIndex >= myPageSize) : (false)))
				{
					myHasUpdated = true;
				}
				else if (myStringIndex >= ((myUsePages) ? ((myPageIndex == -1) ? myText.size() : myPages[myPageIndex].size()) : myText.size())
					&& myUsePages && myCanFlipPage)
				{
					myCanFlipPage = false;

					CountDown delayBeforeFlip;
					delayBeforeFlip.name = "TextTriggerFlipDelay";
					delayBeforeFlip.duration = myPageFlipDelay;
					delayBeforeFlip.callback = [this]() 
					{ 
						myStringIndex = 0; myTempText = ""; myCanFlipPage = true; myPageIndex += 1; 

						if (myPageIndex >= myPageSize)
						{
							if (myOneUse)
								myHasUpdated = true;

							myPageIndex = -1;
							TextComponent* text = myScene->GetComponent<TextComponent>(myTextId);
							text->myText = myText;
						}
					};
					Context::Get()->myTimeHandler->AddTimer(delayBeforeFlip);
				}
				return;
			}

			myTempText += (myUsePages) ? ((myPageIndex == -1) ? myText[myStringIndex] : myPages[myPageIndex][myStringIndex]) : myText[myStringIndex];

			TextComponent* text = myScene->GetComponent<TextComponent>(myTextId);

			text->myText = myTempText;

			myStringIndex++;

			myCanUseNextIndex = false;

			CountDown canPrintNextIndex;
			canPrintNextIndex.name = "TextTriggerAnimationTimer";
			canPrintNextIndex.duration = myWriteSpeed;
			canPrintNextIndex.callback = [this]() { myCanUseNextIndex = true; };
			Context::Get()->myTimeHandler->AddTimer(canPrintNextIndex);
		}

		if (myUsePages && !myUseAnimation && myCanFlipPage)
		{
			myCanFlipPage = false;

			CountDown delayBeforeFlip;
			delayBeforeFlip.name = "TextTriggerFlipDelay";
			delayBeforeFlip.duration = myPageFlipDelay;
			delayBeforeFlip.callback = [this]()
			{
				myCanFlipPage = true; myPageIndex += 1;

				if (myPageIndex >= myPageSize)
				{
					if (myOneUse)
						myHasUpdated = true;

					myPageIndex = -1;
					TextComponent* text = myScene->GetComponent<TextComponent>(myTextId);
					text->myText = myText;
				}
				else
				{
					TextComponent* text = myScene->GetComponent<TextComponent>(myTextId);
					text->myText = myPages[myPageIndex];
				}
			};
			Context::Get()->myTimeHandler->AddTimer(delayBeforeFlip);
		}

		return;
	}

	TransformComponent* transform = myScene->GetComponent<TransformComponent>(myHostId);
	TransformComponent* pTransfrom = myScene->GetComponent<TransformComponent>(myScene->GetPlayerId());

	if (transform == nullptr || pTransfrom == nullptr)
		return;

	float length = (pTransfrom->myPosition - transform->myPosition).Length();


	if (length <= myRadius)
	{
		if (myOneUse && myHasUpdated)
		{
			textTransform->myPosition = CU::Vec3f(-10000.f, -10000.f, 0.f); // BEGONE
			myIsInactive = true;
			myRenderBox = false;
			return;
		}

		myRenderBox = true;

		if (myWillFollow)
		{
			myUnderFollowTimer = true;

			if (myUseAnimation)
			{
				myStringIndex = 0;
				myTempText = "";
			}

			if (myUsePages)
			{
				myPageIndex = -1;
				TextComponent* text = myScene->GetComponent<TextComponent>(myTextId);
				text->myText = myText;
			}

			CountDown canShowText;
			canShowText.name = "TextTriggerFollowTimer";
			canShowText.duration = myFollowTimer;
			canShowText.callback = [this, textTransform]() { myUnderFollowTimer = false; myHasUpdated = true; textTransform->myPosition = CU::Vec3f(-10000.f, -10000.f, 0.f); };
			Context::Get()->myTimeHandler->AddTimer(canShowText);
		}

		if (myUseAnimation)
		{
			if (((myUsePages) ? ((myPageIndex == -1) ? myText.empty() : myPages[myPageIndex].empty()) : myText.empty()) 
				|| ((myUsePages) ? ((myPageIndex == -1) ? myText.size() : myPages[myPageIndex].size()) : myText.size()) <= 1)
				return;

			if (myStringIndex >= ((myUsePages) ? ((myPageIndex == -1) ? myText.size() : myPages[myPageIndex].size()) : myText.size()) 
				|| !myCanUseNextIndex)
			{
				if (myStringIndex >= ((myUsePages) ? ((myPageIndex == -1) ? myText.size() : myPages[myPageIndex].size()) : myText.size()) 
					&& myOneUse && (myUsePages ? (myPageIndex >= myPageSize) : (true)))
				{
					myHasUpdated = true;
				}
				else if (myStringIndex >= ((myUsePages) ? ((myPageIndex == -1) ? myText.size() : myPages[myPageIndex].size()) : myText.size()) 
					&& myUsePages && myCanFlipPage)
				{
					myCanFlipPage = false;

					CountDown delayBeforeFlip;
					delayBeforeFlip.name = "TextTriggerFlipDelay";
					delayBeforeFlip.duration = myPageFlipDelay;
					delayBeforeFlip.callback = [this]() 
					{ 
						myStringIndex = 0; myTempText = ""; myCanFlipPage = true; myPageIndex += 1; 

						if (myPageIndex >= myPageSize)
						{
							if (myOneUse)
								myHasUpdated = true;

							myPageIndex = -1;
							TextComponent* text = myScene->GetComponent<TextComponent>(myTextId);
							text->myText = myText;
						}
					};
					Context::Get()->myTimeHandler->AddTimer(delayBeforeFlip);
				}
				return;
			}

			myTempText += (myUsePages) ? ((myPageIndex == -1) ? myText[myStringIndex] : myPages[myPageIndex][myStringIndex]) : myText[myStringIndex];

			TextComponent* text = myScene->GetComponent<TextComponent>(myTextId);

			text->myText = myTempText;

			myStringIndex++;

			myCanUseNextIndex = false;

			CountDown canPrintNextIndex;
			canPrintNextIndex.name = "TextTriggerAnimationTimer";
			canPrintNextIndex.duration = myWriteSpeed;
			canPrintNextIndex.callback = [this]() { myCanUseNextIndex = true; };
			Context::Get()->myTimeHandler->AddTimer(canPrintNextIndex);
		}

		if (myUsePages && !myUseAnimation && myCanFlipPage)
		{
			myCanFlipPage = false;

			CountDown delayBeforeFlip;
			delayBeforeFlip.name = "TextTriggerFlipDelay";
			delayBeforeFlip.duration = myPageFlipDelay;
			delayBeforeFlip.callback = [this]() 
			{ 
				myCanFlipPage = true; myPageIndex += 1; 

				if (myPageIndex >= myPageSize)
				{
					if (myOneUse)
						myHasUpdated = true;

					myPageIndex = -1;
					TextComponent* text = myScene->GetComponent<TextComponent>(myTextId);
					text->myText = myText;
				}
				else
				{
					TextComponent* text = myScene->GetComponent<TextComponent>(myTextId);
					text->myText = myPages[myPageIndex];
				}
			};
			Context::Get()->myTimeHandler->AddTimer(delayBeforeFlip);
		}

		textTransform->myPosition = myOffset;

		if (myOneUse && !myWillFollow && !myUsePages)
			myHasUpdated = true;

		return;
	}

	if (myUnderFollowTimer)
		return;

	textTransform->myPosition = CU::Vec3f(-10000.f, -10000.f, 0.f); // BEGONE

	myRenderBox = false;

	if (myUseAnimation)
	{
		myStringIndex = 0;
		myTempText = "";
	}

	if (myUsePages)
	{
		myPageIndex = -1;
		TextComponent* text = myScene->GetComponent<TextComponent>(myTextId);
		text->myText = myText;
	}

	if (myOneUse && myHasUpdated)
		myIsInactive = true;
}