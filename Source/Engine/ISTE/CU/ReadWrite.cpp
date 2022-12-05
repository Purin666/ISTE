#include "ReadWrite.h"

#include <fstream>

 char* ReadWrite::myLoadedFile = nullptr;
 size_t ReadWrite::myPosPointer = 0;
 size_t ReadWrite::myFileSize = 0;
 size_t ReadWrite::myMemOffset = 8;

 std::vector<ReadWrite::Data> ReadWrite::myData = std::vector<ReadWrite::Data>();

 size_t ReadWrite::saveDataSize = 0;

void ReadWrite::SaveToFile(std::string aFilePath)
{
	std::ofstream stream(aFilePath, std::ios::binary);

	if (!stream.good())
		return;

	char* writeData = new char[saveDataSize];
	size_t writtenData = 0;

	stream.write((char*)&saveDataSize, sizeof(size_t));

	for (auto& data : myData)
	{
		memcpy(&writeData[writtenData], data.data, data.size);
		writtenData += data.size;
		delete[] data.data;
	}

	stream.write(writeData, saveDataSize);

	myData.clear();
	saveDataSize = 0;

	stream.close();

	delete[] writeData;
}

void ReadWrite::SaveToFile(std::ofstream& aStream)
{
	char* writeData = new char[saveDataSize];
	size_t writtenData = 0;

	aStream.write((char*)&saveDataSize, sizeof(size_t));

	for (auto& data : myData)
	{
		memcpy(&writeData[writtenData], data.data, data.size);
		writtenData += data.size;
		delete[] data.data;
	}

	aStream.write(writeData, saveDataSize);

	myData.clear();
	saveDataSize = 0;

	delete[] writeData;
}

bool ReadWrite::LoadFile(std::string aFilePath, size_t aStreamOffset)
{
	std::ifstream stream(aFilePath, std::ios::binary);

	stream.seekg(aStreamOffset);

	if (!stream.good())
		return false;

	stream.read((char*)&myFileSize, sizeof(size_t));

	myLoadedFile = new char[myFileSize];

	stream.read(myLoadedFile, myFileSize);

	stream.close();

	return true;

}

bool ReadWrite::LoadFileX(std::string aFilePath, size_t aFileSize)
{
	std::ifstream stream(aFilePath, std::ios::binary);

	if (!stream.good())
		return false;

	myLoadedFile = new char[aFileSize];

	stream.read(myLoadedFile, aFileSize);

	stream.close();

	return true;
}

void ReadWrite::Flush()
{
	delete[] myLoadedFile;
	myFileSize = 0;
	myPosPointer = 0;
}
