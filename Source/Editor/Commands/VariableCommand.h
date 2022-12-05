#pragma once
#include "AbstractCommand.h"
template <typename T>
class VariableCommand : public AbstractCommand {
private:
	T* myDestination;
	const T myOrigin;
	const T myResult;
public:
	template <typename T>
	VariableCommand(
		T* aDestination,
		T aOrigin, T aResult
	) : myDestination(aDestination)
		, myOrigin(aOrigin)
		, myResult(aResult)
	{}
	~VariableCommand() {};

	bool Execute() override
	{
		*myDestination = myResult;
		return false;
	};
	bool Undo() override
	{
		*myDestination = myOrigin;
		return false;
	};


};