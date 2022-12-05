#pragma once
#include "AbstractTool.h"
namespace ISTE
{

class PPEffectsTool : public AbstractTool
{
public:
	void Init(BaseEditor*) override;
	void Draw() override;

private:
	void PPSliders();
	void PPSelect();
	void PPSet();
	void PPAddNew();
	void PPRename();
	void PPRemove();
	void PPBlending();

	bool myPPBlendingIsOpen = false;
	bool myAddNewPresetIsOpen = false;
	bool myRenamePresetIsOpen = false;
	bool myRemovePresetIsOpen = false;

	size_t mySelectedPPIndex = 0;
	size_t myFirstPPBlendIndex = 0;
	size_t mySecondPPBlendIndex = 0;
	size_t myRemovePPIndex = 0;
	bool myIsBlendingManual = false;
	bool myIsBlendingOverTime = false;
	float myBlendValue = 0.f;
	char myCharArrayForPresetNames[16];

};
}