// MADE BY MARTIN NILSSON 2022

#pragma once

#include "Json/json.hpp"

#include <fstream>
#include <iostream>
#include <mutex>
#include <string>

namespace ISTE
{
	static std::mutex globalCoutMutex;

	template <class T>
	bool LoadJson(const std::string& aPath, T& someOutData)
	{
		std::ifstream i(aPath);

		if (!i.is_open())
		{
			std::lock_guard<std::mutex> lg(globalCoutMutex);
			std::cout << "Error in " << __func__ << ": Could not open " << aPath << std::endl;
			return false;
		}

		try
		{
			nlohmann::json j;
			i >> j;
			someOutData = j.get<T>();
			return true;
		}
		catch (const std::exception& e)
		{
			std::lock_guard<std::mutex> lg(globalCoutMutex);
			std::cout << "Error parsing " << aPath << ": " << e.what() << std::endl;
			return false;
		}
	}

	template <class T>
	void SaveJson(const std::string& aPath, const T& someData)
	{
		std::ofstream o(aPath);
		
		if (!o.is_open())
		{
			std::lock_guard<std::mutex> lg(globalCoutMutex);
			std::cout << "Error in " << __func__ << ": Could not open " << aPath << std::endl;
			return;
		}
		try
		{
			nlohmann::json j = someData;
			o << std::setw(4) << j;
			return;
		}
		catch (const std::exception& e)
		{
			std::lock_guard<std::mutex> lg(globalCoutMutex);
			std::cout << "Error parsing " << aPath << ": " << e.what() << std::endl;
			return;
		}
	}
}