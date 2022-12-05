#pragma once
#include "NavMeshDefines.h"

#include <fstream>
#include <sstream>
#include <cassert>

#include <iostream>
#include <chrono>
#include <ctime>    

#include "ISTE/CU/MemTrack.hpp"

namespace ISTE
{
	namespace NavMesh
	{
		static void LoadNavMesh(const std::string& aPath, NavMesh& aNavMesh)
		{
			std::fstream file(aPath);

			if (!file.is_open())
				return;

			std::vector<CU::Vec3f> vertices;
			std::vector<std::array<unsigned int, 3>> faces;

			while (!file.eof())
			{
				char line[128];
				file.getline(line, 128);

				std::stringstream ss;
				ss << line;

				char junk;

				if (line[0] == 'v')
				{
					float v[3];
					ss >> junk >> v[0] >> v[1] >> v[2];
					vertices.push_back({ v[0], v[1], v[2] });
				}

				if (line[0] == 'f')
				{
					unsigned int f[3];
					ss >> junk >> f[0] >> f[1] >> f[2];
					faces.push_back({ f[0], f[1], f[2] });
				}
			}

			aNavMesh.triangles.resize(faces.size());
			aNavMesh.vertices = vertices;

			for (size_t i = 0; i < faces.size(); ++i)
			{
				aNavMesh.triangles[i] = {
					{
						&aNavMesh.vertices[faces[i][0]],
						&aNavMesh.vertices[faces[i][1]],
						&aNavMesh.vertices[faces[i][2]]
					},
					faces[i],
					(int)i };
			}

			auto& tris = aNavMesh.triangles;
			size_t count = 0;
			for (size_t i = 0; i < tris.size(); i++)
			{
				for (size_t k = i + 1; k < tris.size(); k++)
				{
					count = 0;
					if (tris[i].connections.size() >= 3)
						break;
					if (tris[k].connections.size() >= 3)
						continue;

					if (tris[i].vertexIndices[0] == tris[k].vertexIndices[0]) count++;
					if (tris[i].vertexIndices[0] == tris[k].vertexIndices[1]) count++;
					if (tris[i].vertexIndices[0] == tris[k].vertexIndices[2]) count++;

					if (tris[i].vertexIndices[1] == tris[k].vertexIndices[0]) count++;
					if (tris[i].vertexIndices[1] == tris[k].vertexIndices[1]) count++;
					if (tris[i].vertexIndices[1] == tris[k].vertexIndices[2]) count++;

					if (tris[i].vertexIndices[2] == tris[k].vertexIndices[0]) count++;
					if (tris[i].vertexIndices[2] == tris[k].vertexIndices[1]) count++;
					if (tris[i].vertexIndices[2] == tris[k].vertexIndices[2]) count++;

					if (count == 2)
					{
						tris[i].connections.push_back(k);
						tris[k].connections.push_back(i);
					}
					if (count > 2)
						std::cout << "WARNING: Two NavMeshTriangles have more than than two shared vertices" << std::endl;
				}
			}

			// WIP:: sort the navmesh triangles in y axis
			//const size_t length = aNavMesh.triangles.size() - 1;
			//for (size_t i = 0; i < length; ++i)
			//{
			//	for (size_t j = 0; j < length - i; ++j)
			//	{
			//		const float a = aNavMesh.triangles[j + 1].Center().y;
			//		const float b = aNavMesh.triangles[j].Center().y;

			//		if (a < b)
			//		{
			//			std::swap(aNavMesh.triangles[j], aNavMesh.triangles[j + 1]);
			//		}
			//	}
			//}

		}
		namespace
		{
			std::stringstream GetInformation(const NavMesh& aNavMesh)
			{
				std::stringstream ssDate;
				std::stringstream ssTime;
				std::stringstream result;

				time_t t = time(0);
				tm* LT = localtime(&t);
				LT->tm_year += 1900;
				LT->tm_mon += 1;

				ssDate << LT->tm_year << "-";
				ssDate << LT->tm_mon << "-";
				if (LT->tm_mday < 10) ssDate << 0;
				ssDate << LT->tm_mday;

				if (LT->tm_hour < 10) ssTime << 0;
				ssTime << LT->tm_hour << ":";
				if (LT->tm_min < 10) ssTime << 0;
				ssTime << LT->tm_min << ":";
				if (LT->tm_sec < 10) ssTime << 0;
				ssTime << LT->tm_sec;

				result << "# This mesh was edited in ISTE\n";
				result << "# Date:\t" << ssDate.str() << "\n";
				result << "# Time:\t" << ssTime.str() << "\n";
				result << "# User:\t" << getenv("username") << "\n";
				result << "# Vertices:\t" << std::to_string(aNavMesh.vertices.size()) << "\n";
				result << "# Faces:\t" << std::to_string(aNavMesh.triangles.size()) << "\n";

				return result;
			}
		}
		static void SaveNavMesh(const std::string& aPath, const NavMesh& aNavMesh)
		{
			std::stringstream ss;
			char _ = ' ';
			std::array<unsigned int, 3> f;

			ss << GetInformation(aNavMesh).str();

			ss << "\n# Vertices\n";
			for (const CU::Vec3f& v : aNavMesh.vertices)
			{
				ss << "v" << _ << v.x << _ << v.y << _ << v.z << "\n";
			}
			ss << "\n# Faces\n";
			for (const NavMeshTriangle& tri : aNavMesh.triangles)
			{
				f = tri.vertexIndices;
				ss << "f" << _ << f[0] << _ << f[1] << _ << f[2] << "\n";
			}

			std::ofstream file;
			file.open(aPath);
			file << ss.rdbuf();
			file.flush();
			file.close();
		}

	} // NavMesh
} // ISTE