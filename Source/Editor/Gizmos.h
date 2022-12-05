#pragma once

#include "ISTE/ECSB/ECSDefines.hpp"
#include "ISTE/Math/Vec3.h"
#include "ISTE/Math/Euler.h"

#include <Windows.h>

namespace ISTE
{
	enum class GizmosOptions
	{
		Local = 0,
		World = 1,
		Translate = 7,
		Rotate = 120,
		Scale = 896
	};

	class Gizmos
	{
	public:
		Gizmos();

		inline void Activate(EntityID aEntityId) { myEntity = aEntityId; }
		inline void DeActivate() { myEntity = INVALID_ENTITY; }

		inline void SetDrawMode(GizmosOptions anOption) { mySelectedOption = anOption; }

		inline void BoxVisual(bool aVisual) { myVisualBox = aVisual; }

		inline void SetSnapAmount(float aSnapAmount) { mySnap[0] = aSnapAmount; mySnap[1] = aSnapAmount; mySnap[2] = aSnapAmount;}
		void CheckInput(bool aDisplayActive);

		//returns true if gizmos was pressed
		bool Draw(RECT aDisplayRect, bool aDisplayActive);
	private:
		char myTranslationKey = 'W';
		char myRotationKey = 'E';
		char myScaleKey = 'R';

		bool myVisualBox = false;

		EntityID myEntity = INVALID_ENTITY;
		GizmosOptions mySelectedOption = GizmosOptions::Translate;
		GizmosOptions mySpaceOption = GizmosOptions::World;
		float mySnap[3] = { 0.0f, 0.0f, 0.0f };
		float mySnapValue = 0.25f;
		CU::Vec3f myOriginPos;
		CU::Vec3f myOriginSize;
		CU::Euler myOriginRot;
	};
}