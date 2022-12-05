#include "NavMeshImporter.h"

#include <fstream>
#include <sstream>
#include <cassert>

#include "ISTE/CU/MemTrack.hpp"

ISTE::NavMeshImporter::NavMeshImporter(std::string aPath)
{



	std::fstream file(aPath);
	//assert(file.is_open() && L"Can't open file");

	myIsSafe = false;

	if (!file.is_open())
		return;

	myTriangles; 
	while (!file.eof())
	{
		char line[128];
		file.getline(line, 128);

		std::stringstream ss;
		ss << line;

		char junk;

		if (line[0] == 'v')
		{ 
			float pos[3];
			ss >> junk >> pos[0] >> pos[1] >> pos[2];
			myPositions.push_back(CU::Vec3f(pos[0], pos[1], pos[2]));
		}

		if (line[0] == 'f')
		{
			int face[3];
			ss >> junk >> face[0] >> face[1] >> face[2]; 
			myTriangles.push_back(CU::Vec3i( face[0], face[1], face[2] ));
		}
	} 


	myIsSafe = true;


	//replaced it with the above algorithm mmm				!
	//myIsSafe = true;
	//
	//std::ifstream read(aPath);
	//
	//if (read)
	//{
	//	read.seekg(0, read.end);
	//	int length = read.tellg();
	//	read.seekg(0, read.beg);
	//
	//	char* buffer = new char[length];
	//
	//	std::cout << "Reading " << length << " characters... " << std::endl;
	//
	//	read.read(buffer, length);
	//
	//	if (read)
	//	{
	//		// Success
	//		for (int i = 0; i < length; i++)
	//		{
	//			if (buffer[i] == 'v') // All Vertices are saved.
	//			{
	//				int y = i + 2;
	//				std::string fNum = "";
	//				
	//				std::vector<float> tempVec;					
	//				tempVec.reserve(3);
	//				
	//				// Pushing back empty numbers.
	//				{ 
	//					tempVec.push_back(0.f);
	//					tempVec.push_back(0.f);
	//					tempVec.push_back(0.f);
	//				} 
	//
	//				int index = 0;
	//				
	//				while (buffer[y] != 'v' && buffer[y] != 'f' && buffer[y] != 'e')
	//				{
	//					
	//
	//					if (buffer[y] == ',')
	//						fNum += '.';
	//					else
	//						fNum += buffer[y];
	//
	//					y++;
	//
	//
	//					if (buffer[y] == '|' || buffer[y] == 'v')
	//					{
	//						tempVec[index] = std::stof(fNum);
	//						index++;
	//						
	//						if (buffer[y] == '|')
	//							y++;
	//
	//						fNum = "";
	//					}
	//				}					
	//				myPositions.push_back(CU::Vec3f(tempVec[0], tempVec[1], tempVec[2]));
	//				i = y - 1; // Move back to y value when going through for loop.
	//			}
	//			else if (buffer[i] == 'f') // All triangles indexes are saved.
	//			{
	//				int y = i + 2;
	//
	//				std::string iNum = "";
	//
	//				std::vector<int> tempVec;
	//				tempVec.reserve(3);
	//
	//				// Pushing back empty numbers.
	//				{
	//					tempVec.push_back(0.f);
	//					tempVec.push_back(0.f);
	//					tempVec.push_back(0.f);
	//				}
	//
	//				int index = 0;
	//
	//				while (buffer[y] != 'v' && buffer[y] != 'f' && buffer[y] != 'e')
	//				{
	//
	//					iNum += buffer[y];
	//
	//					y++;
	//
	//					if (buffer[y] == '|' || buffer[y] == 'f' || buffer[y] == 'e')
	//					{
	//						tempVec[index] = std::stoi(iNum);
	//						index++;
	//
	//						if (buffer[y] == '|')
	//							y++;
	//
	//						iNum = "";
	//					}
	//				}
	//				myTriangles.push_back(CU::Vec3i(tempVec[0], tempVec[1], tempVec[2]));
	//				i = y - 1; // Move back to y value when going through for loop.
	//			}
	//		}
	//	}
	//	else
	//	{
	//		myIsSafe = false;
	//		std::cout << "Error: only " << read.gcount() << " could be read" << std::endl;
	//	}
	//	read.close();
	//	delete[] buffer;
	//}
	//else
	//	myIsSafe = false;
}

ISTE::NavMeshImporter::~NavMeshImporter()
{
}