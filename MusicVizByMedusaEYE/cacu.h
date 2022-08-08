#ifndef CACU_H
#define CACU_H

#include"../../../BA/Cpp/BA_1_2/BA_1_2/BA_1_2.hpp"
#include"SD.h"

BA_Array CacuInterAndSlope(BA_Array dotsAX, BA_Array dotsAY, BA_Array dotsBX, BA_Array dotsBY);

BA_Array DrawLinesWithXY(Dots* dotsA, Dots* dotsB, float loopTime, balist<float>& dotsAP, balist<float>& dotsBP);

Dots CacuIntercetingDots(BA_Array linesIntercept, BA_Array linesSlope, int i);


#endif