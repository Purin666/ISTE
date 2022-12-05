#pragma once
//#include <vector>
//#include <queue>
//#include "ISTE/Interpolation/InterpolationDefines.h"
//
//
//namespace ISTE {
//
//	class InterpolationManager
//	{
//	public:
//		void								Update					();
//		float								GetInterpValue			(InterpolationID);
//		InterpolationData::ConstantData&	GetConstantInterpData	(InterpolationID);
//		InterpolationData::LinearData&		GetLinearInterpData		(InterpolationID);
//		InterpolationData::SinData&			GetSinData				(InterpolationID);
//		InterpolationID						Create					(InterpolationData::ConstantData, InterpolationFlag);
//		InterpolationID						Create					(InterpolationData::LinearData, InterpolationFlag);
//		InterpolationID						Create					(InterpolationData::SinData, InterpolationFlag); 
//		void								Remove					(InterpolationID);
//	private:
//		void TickLinear	();
//		void TickSin	();
//		void CalcLinear	();
//		void CalcSin	();
//
//
//		std::queue<size_t> myNextFreeSpotConstant;
//		std::vector<InterpolationData::ConstantData>	myConstantData;
//
//		std::queue<size_t> myNextFreeSpotLinear;
//		std::vector<InterpolationData::LinearData>		myLinearData;
//		
//		std::queue<size_t> myNextFreeSpotSin;
//		std::vector<InterpolationData::SinData>			mySinData;
//	};
//};