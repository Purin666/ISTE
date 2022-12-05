#pragma once
#include <string>

namespace ISTE {
	class Logger
	{
	public: 
		static int  GetDrawCalls() { return myLastDrawCalls; }
		static int  GetBatchedDrawCalls() { return myLastBatchedDrawCalls; }
		static void LoggDrawCall() { myDrawCalls++; }
		static void LoggBatchedDrawCall() { myBatchedDrawCalls++; }
		static void FlushDrawCall() { myLastDrawCalls = myDrawCalls; myDrawCalls = 0; 
									  myLastBatchedDrawCalls = myBatchedDrawCalls; myBatchedDrawCalls = 0;}

		static void Popup(const std::wstring& aCaption, const std::wstring& aMsg);
		static void PopupError(const std::wstring& aCaption, const std::wstring& aMsg);

		static void DebugOutputLogg(const wchar_t*);
		static void DebugOutputWarning(const wchar_t*);
		static void DebugOutputError(const wchar_t*);

		static void DebugOutputLogg(std::wstring&);
		static void DebugOutputWarning(std::wstring&);
		static void DebugOutputError(std::wstring&);

		static void CoutError(const char* );
		static void CoutWarning(const char*);
		static void CoutSuccess(const char*);

		static void CoutError(const std::string&);
		static void CoutWarning(const std::string&);
		static void CoutSuccess(const std::string&);


	private:
		inline static int myDrawCalls;
		inline static int myBatchedDrawCalls;
		inline static int myLastDrawCalls;
		inline static int myLastBatchedDrawCalls;
	};
}
