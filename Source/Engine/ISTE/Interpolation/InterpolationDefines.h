#pragma once
//
//typedef unsigned char InterpolationFlag;
//
//#define INTERP_PLAYING					0b00000100
//#define INTERP_LOOP						0b00000010
//#define INTERP_DESTROY_ON_COMPLETION	0b00000001
//
//namespace ISTE {
//
//
//	enum class InterpolationType
//	{
//		eConstant,
//		eLinear,
//		eSin,
//		eCount
//		//mwba
//	};
//
//	struct InterpolationID {
//		InterpolationType	myType;
//		size_t				myDataIndex;
//	};
//	
//	namespace InterpolationData {
//
//		struct ConstantData { 
//			float myTValue = 0;
//
//			InterpolationFlag myFlag;
//			float myTimer = 0;
//		};
//		//f(x) = x * mySpeed
//		struct LinearData {
//			float mySpeed;		//X ( R
//			float myTValue = 0;
//			
//			InterpolationFlag myFlag;
//			float myTimer = 0;
//		};
//
//		//f(x) = (0.5 + sin(x*mySpeed) * 0.5) * myModifier;
//		struct SinData {
//			float mySpeed;		//X ( R
//			float myModifier;	//0 >= X <= 1
//			float myTValue = 0;
//			
//			InterpolationFlag myFlag;
//			float myTimer = 0;
//		};
//	}; 
//};
//