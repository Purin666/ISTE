#include "Logger.h"
#include <Windows.h>
#include <iostream>

void ISTE::Logger::Popup(const std::wstring& aCaption, const std::wstring& aMsg)
{
	MessageBox(nullptr, aMsg.c_str(), aCaption.c_str(), MB_OK | MB_ICONASTERISK);
}

void ISTE::Logger::PopupError(const std::wstring& aCaption, const std::wstring& aMsg)
{
	MessageBox(nullptr, aMsg.c_str(), aCaption.c_str(), MB_OK | MB_ICONERROR);
}

void ISTE::Logger::DebugOutputLogg(const wchar_t* aMsg)
{
	DebugOutputLogg(std::wstring(aMsg));
}

void ISTE::Logger::DebugOutputWarning(const wchar_t* aMsg)
{
	DebugOutputWarning(std::wstring(aMsg));
}

void ISTE::Logger::DebugOutputError(const wchar_t* aMsg)
{
	DebugOutputError(std::wstring(aMsg));
}

void ISTE::Logger::DebugOutputLogg(std::wstring& aMsg)
{
	aMsg = L"Engine Log " + aMsg + L"\n";
	OutputDebugStringW(aMsg.c_str());
}

void ISTE::Logger::DebugOutputWarning(std::wstring& aMsg)
{
	aMsg = L"Engine Warning " + aMsg + L"\n";
	OutputDebugStringW(aMsg.c_str());
}

void ISTE::Logger::DebugOutputError(std::wstring& aMsg)
{
	aMsg = L"Engine Error - " + aMsg + L"\n";
	OutputDebugStringW(aMsg.c_str());
}

void ISTE::Logger::CoutError(const char* aMsg)
{
	CoutError(std::string(aMsg));
}

void ISTE::Logger::CoutWarning(const char* aMsg)
{
	CoutWarning(std::string(aMsg));
}

void ISTE::Logger::CoutSuccess(const char* aMsg)
{
	CoutSuccess(std::string(aMsg));
}

void ISTE::Logger::CoutError(const std::string& aMsg)
{
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(console, FOREGROUND_RED | FOREGROUND_INTENSITY); 
	std::cout << aMsg << std::endl;
	SetConsoleTextAttribute(console, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);

}

void ISTE::Logger::CoutWarning(const std::string& aMsg)
{
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(console, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
	std::cout << aMsg << std::endl;
	SetConsoleTextAttribute(console, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
}

void ISTE::Logger::CoutSuccess(const std::string& aMsg)
{
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(console, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
	std::cout << aMsg << std::endl;
	SetConsoleTextAttribute(console, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
}
