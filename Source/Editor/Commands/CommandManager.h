#pragma once

#include <memory>
#include <stack>

class AbstractCommand;

class CommandManager {
public:
	static void DoCommand(AbstractCommand* command);
	static void Undo();
	static void Redo();
	static void Empty();

private:
	static std::stack<AbstractCommand*> myUndoStack;
	static std::stack<AbstractCommand*> myRedoStack;
};