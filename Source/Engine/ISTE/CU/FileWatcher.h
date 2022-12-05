#pragma once

#include <filesystem>

#include <vector>
#include <string>

namespace CU
{

	template<typename T, bool CheckSubFolders = true>
	class FileWatcher
	{
	public:
		void SetCallbackFunc(T* aSender, void(T::* aCallback)(std::vector<std::string>, std::vector<std::string>));
		void AddDirectoryPath(const char* aPath);
		void CheckForChanges();
		void Reconstruct();
		void VerifyDirectotyState(std::vector<std::string>& someContent, std::vector<std::string>& someDirectorys);
		inline void AddExtensionCheck(std::string aExtension)
		{
			myExtensionChecks.push_back(aExtension);
		}

		//new ting
		inline void SetFolderCallbackFunc(void(T::* aCallback)(std::vector<std::string>)) { myFolderCallback = aCallback; }
		inline void SetFolderChangedCallbackFunc(void(T::* aCallback)()) { myFolderChangedCallback = aCallback; }
		void CheckFolder(std::string aFolderPath, const std::vector<std::string>& aFolderCheck, const std::vector<std::string>& aContentCheck);


		//

	private:
		//new ting
		void(T::* myFolderCallback)(std::vector<std::string>) = nullptr;
		void(T::* myFolderChangedCallback)() = nullptr;
		//

		void(T::* myCallback)(std::vector<std::string>, std::vector<std::string>) = nullptr;
		T* mySenderInstance = nullptr;
		std::vector<std::string> myCheckDirectorys;
		std::vector<std::string> myDirectoryContent;
		std::vector<std::string> myExtensionChecks;




	};



	template<typename T, bool CheckSubFolders>
	inline void FileWatcher<T, CheckSubFolders>::SetCallbackFunc(T* aSender, void(T::* aCallback)(std::vector<std::string>, std::vector<std::string>))
	{
		myCallback = aCallback;
		mySenderInstance = aSender;
	}
	template<typename T, bool CheckSubFolders>
	inline void FileWatcher<T, CheckSubFolders>::AddDirectoryPath(const char* aPath)
	{
		myCheckDirectorys.push_back(aPath);
	}

	template<typename T, bool CheckSubFolders>
	inline void FileWatcher<T, CheckSubFolders>::Reconstruct()
	{
		std::vector<std::string> currentDirectoryState;

		std::vector<std::string> paths;

		if (CheckSubFolders)
		{
			for (auto& dC : myCheckDirectorys)
			{
				paths.push_back(dC);
				for (auto& path : std::filesystem::recursive_directory_iterator(dC))
				{

					if (!path.is_directory())
						continue;



					paths.push_back(path.path().u8string());
				}
			}

		}
		else
		{
			paths = myCheckDirectorys;
		}

		for (auto& dC : paths)
		{

			//catch invalid string and ignore
			for (auto& path : std::filesystem::directory_iterator(dC))
			{
				if (path.is_directory() || path.path().u8string() == "")
					continue;

				currentDirectoryState.push_back(path.path().u8string());
			}
		}

		(*mySenderInstance.*myCallback)(currentDirectoryState, paths);
	}

	template<typename T, bool CheckSubFolders>
	inline void FileWatcher<T, CheckSubFolders>::CheckForChanges()
	{
		std::vector<std::string> currentDirectoryState;

		std::vector<std::string> paths;

		if (CheckSubFolders)
		{
			for (auto& dC : myCheckDirectorys)
			{
				paths.push_back(dC);
				for (auto& path : std::filesystem::recursive_directory_iterator(dC))
				{

					if (!path.is_directory())
						continue;



					paths.push_back(path.path().u8string());
				}
			}

		}
		else
		{
			paths = myCheckDirectorys;
		}

		for (auto& dC : paths)
		{
			
			//catch invalid string and ignore
			for (auto& path : std::filesystem::directory_iterator(dC))
			{
				if (path.is_directory() || path.path().u8string() == "")
					continue;

				//std::string extension = path.path().extension().u8string();

				//for (auto& ex : myExtensionChecks)
				//{
				//	if (extension == ex)
				//	{
				//		currentDirectoryState.push_back(path.path().u8string());
				//		break;
				//	}
				//}


				currentDirectoryState.push_back(path.path().u8string());
			}
		}

		VerifyDirectotyState(currentDirectoryState, paths);
	}

	template<typename T, bool CheckSubFolders>
	inline void FileWatcher<T, CheckSubFolders>::VerifyDirectotyState(std::vector<std::string>& someContent, std::vector<std::string>& someDirectorys)
	{
		bool fileNotFound = false;

		//this hole thing is very ugly

		if (myDirectoryContent.size() != someContent.size())
		{
			fileNotFound = true;
		}
		else
		{
			for (auto& currentContent : someContent)
			{
				bool found = false;
				for (auto& lastContent : myDirectoryContent)
				{
					if (currentContent == lastContent)
					{
						found = true;
						break;
					}
				}

				if (!found)
				{
					fileNotFound = true;
					break;
				}
			}
		}


		myDirectoryContent.clear();
		myDirectoryContent = someContent;
		//for (auto& c : someContent)
		//{
		//	myDirectoryContent.push_back(c);
		//}

		if (fileNotFound)
			(*mySenderInstance.*myCallback)(myDirectoryContent, someDirectorys);

	}


	template<typename T, bool CheckForSubFolders>
	inline void FileWatcher<T, CheckForSubFolders>::CheckFolder(std::string aFolderPath, const std::vector<std::string>& aFolderCheck, const std::vector<std::string>& aContentCheck)
	{
		std::vector<std::string> currentContent;
		std::vector<std::string> currentFolders;

		for (auto& path : std::filesystem::directory_iterator(aFolderPath))
		{
			if (path.is_directory())
			{
				currentFolders.push_back(path.path().u8string());
				continue;
			}

			currentContent.push_back(path.path().u8string());
		}

		if (currentFolders.size() != aFolderCheck.size())
		{
			(*mySenderInstance.*myFolderChangedCallback)();
			return;
		}

		if (currentContent.size() != aContentCheck.size())
		{
			(*mySenderInstance.*myFolderCallback)(currentContent);
			return;
		}

		for (auto& folder : currentFolders)
		{
			bool foundFolder = false;
			for (auto& f : aFolderCheck)
			{
				if (f == folder)
				{
					foundFolder = true;
					break;
				}
			}

			if (!foundFolder)
			{
				(*mySenderInstance.*myFolderChangedCallback)();
				return;
			}
		}

		for (auto& content : currentContent)
		{
			bool foundContent = false;
			for (auto& c : aContentCheck)
			{
				if (c == content)
				{
					foundContent = true;
					break;
				}
			}

			if (!foundContent)
			{
				(*mySenderInstance.*myFolderCallback)(currentContent);
				break;
			}
		}
	}
}
