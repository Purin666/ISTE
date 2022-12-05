#pragma once

#include "ISTE/StateStack/State.h"
#include "ISTE/UI/UIDefines.h"

#include "ISTE/Time/LocalCountDown.h"

#include <array>

namespace ISTE
{
	class UI;
	class PlayerBehaviour;
	class BossBobBehaviour;

	class State_HUD : public State
	{
	public:
		~State_HUD();

		virtual void Init()				 override;
		virtual void Update(const float) override;
		virtual void Render()			 override;

		void ProcessAbilityFades();
		void ProcessAbilityOutOfMana();
		void ProcessHeahlthAndMana(const float, const float);
		void ProcessExperience(const float);
		void ProcessBossHealth(const float, const float);

		void EnableAbility(const int);

	private:
		UI* myPlayerHUD = nullptr;
		UI* myBossHUD = nullptr;

		GameplayImage* myPlayerHeathImage = nullptr;
		GameplayImage* myPlayerManaImage = nullptr;
		GameplayImage* myPlayerExperienceImage = nullptr;
		GameplayImage* myBossHealthImage = nullptr;
		Sprite2DRenderCommand myPlayerHealthImageOG;
		Sprite2DRenderCommand myPlayerManaImageOG;
		Sprite2DRenderCommand myPlayerExperienceImageOG;
		Sprite2DRenderCommand myBossHealthImageOG;
		float myPlayerHealthLerpElapsedTime = 0.f;
		float myPlayerManaLerpElapsedTime = 0.f;
		float myExperienceLerpElapsedTime = 0.f;
		float myBossHealthLerpElapsedTime = 0.f;

		std::array<Sprite2DRenderCommand, 6> myAbilityFades;
		std::array<Sprite2DRenderCommand, 6> myAbilityFadesOG;
		std::array<Sprite2DRenderCommand, 6> myLockedIcons;

		std::array<float, 6> myElapsedTimes;
		std::array<float, 6> myCDTimings;
		std::array<int, 6> myCDVFXIDs;
		float myElapsedTime = 0.f;

		std::array<LocalCountDown, 6> myCooldownTimers;

		int myHealthParticlesID = -1;
		int myManaParticlesID = -1;

		int myAbilityEnable = 0;

		PlayerBehaviour* myPlayer = nullptr;
		BossBobBehaviour* myBoss = nullptr;

	};
}