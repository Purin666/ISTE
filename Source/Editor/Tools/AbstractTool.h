#pragma once
#include <string>
namespace ISTE
{
	class BaseEditor;
	class AbstractTool
	{
	public:
		virtual ~AbstractTool() = default;
		virtual void Init(BaseEditor*) { return; }
		void CheckActive();
		virtual void Draw() { return; }
		virtual void Settings();
	protected:
		bool myActive = false;
		std::string myToolName = "NULL";
	};
}