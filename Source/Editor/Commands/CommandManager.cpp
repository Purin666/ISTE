#include "CommandManager.h"

#include "AbstractCommand.h"
#include "ISTE/CU/MemTrack.hpp"

std::stack<AbstractCommand*> CommandManager::myUndoStack;
std::stack<AbstractCommand*> CommandManager::myRedoStack;

void CommandManager::DoCommand(AbstractCommand *command) {
	myUndoStack.push(command);
	while (myRedoStack.size() != 0)
	{
		AbstractCommand* command = myRedoStack.top();
		myRedoStack.pop();
		delete command;
	}
}

void CommandManager::Undo() {
	if (myUndoStack.size() > 0)
	{
		myRedoStack.push(myUndoStack.top());
		myUndoStack.top()->Undo();
		myUndoStack.pop();
	}
}

void CommandManager::Redo() {
	if (myRedoStack.size() > 0)
	{
		myUndoStack.push(myRedoStack.top());
		myRedoStack.top()->Execute();
		myRedoStack.pop();
	}
}

void CommandManager::Empty()
{
	while (!myRedoStack.empty())
	{
		AbstractCommand* command = myRedoStack.top();
		myRedoStack.pop();
		delete command;
	}
	while (!myUndoStack.empty())
	{
		AbstractCommand* command = myUndoStack.top();
		myUndoStack.pop();
		delete command;
	}
}
