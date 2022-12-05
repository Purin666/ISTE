#pragma once

#include <string>

#define StyleOptions 3

class EditorSettings
{
public:
	EditorSettings();
	void Activate() { myIsActive = true; }
	void Draw();

private:
	void CustomStyle();

private:
	bool myIsActive = false;

	//style
	bool myCreateStyle = false;
};