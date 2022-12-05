#pragma once

#include <string>
#include <vector>

class ReadWrite
{
public:
	//Save
	template<typename T>
	static void Que(const T& aType)
	{
		Data data;
		data.size = sizeof(T);
		data.data = new char[data.size];
		memcpy(data.data, &aType, data.size);

		myData.push_back(data);

		saveDataSize += data.size;
	}
	inline static void Que(const void* someData, int aDataSize)
	{
		Data data;
		data.size = aDataSize;
		data.data = new char[aDataSize];
		memcpy(data.data, someData, aDataSize);
		myData.push_back(data);

		saveDataSize += aDataSize;
	}
	template<typename T>
	static void QueV(const char* aType)
	{
		Data data;
		data.size = sizeof(T) - myMemOffset;
		data.data = new char[data.size];
		memcpy(data.data, &aType[myMemOffset], data.size);

		myData.push_back(data);

		saveDataSize += data.size;
	}
	inline static void QueV(const char* someData, size_t aDataSize)
	{
		Data data;
		data.size = aDataSize - myMemOffset;
		data.data = new char[data.size];
		memcpy(data.data, &someData[myMemOffset], data.size);
		myData.push_back(data);

		saveDataSize += aDataSize;
	}

	static void SaveToFile(std::string aFilePath);
	static void SaveToFile(std::ofstream& aStream);
	
	//Load
	template<typename T>
	static void Read(T& aType)
	{
		memcpy(&aType, &myLoadedFile[myPosPointer], sizeof(T));
		myPosPointer += sizeof(T);
	}
	inline static void Read(void* aDataPtr, int aDataSize)
	{
		memcpy(aDataPtr, &myLoadedFile[myPosPointer], aDataSize);
		myPosPointer += aDataSize;
	}
	template<typename T>
	static void ReadV(char* aType)
	{
		memcpy(&aType[myMemOffset], &myLoadedFile[myPosPointer], sizeof(T) - myMemOffset);
		myPosPointer += sizeof(T) - myMemOffset;
	}
	inline static void ReadV(char* aDataPtr, int aDataSize)
	{
		memcpy(&aDataPtr[myMemOffset], &myLoadedFile[myPosPointer], aDataSize - myMemOffset);
		myPosPointer += aDataSize - myMemOffset;
	}

	static bool LoadFile(std::string aFilePath, size_t aStreamOffset = 0);
	static bool LoadFileX(std::string aFilePath, size_t aFileSize);
	static void Flush();

	inline static char* GetLoadedData() { return myLoadedFile; }
	inline static void SetMemOffset(size_t aOffset) { myMemOffset = aOffset; }

private:
	struct Data
	{
		char* data = nullptr;
		size_t size = 0;
	};

	//derived
	static size_t myMemOffset;

	//save
	static std::vector<Data> myData;
	static size_t saveDataSize;

	//Load
	static char* myLoadedFile;
	static size_t myPosPointer;
	static size_t myFileSize;
};