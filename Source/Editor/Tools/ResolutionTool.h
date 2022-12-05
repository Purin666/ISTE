#pragma once
#include "AbstractTool.h"
namespace ISTE
{


class ResolutionTool : public AbstractTool
{
public:
	void Init(BaseEditor*) override;
	void Draw() override;


private:



};
}