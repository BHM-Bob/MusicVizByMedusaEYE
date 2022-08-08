#include"../../../BA/Cpp/BA_1_2/BA_1_2/BA_1_2.hpp"
#include"SD.h"
#include "cacu.h"

// func for cacu y = a * x + b from(x1, y1) and (x2, y2)
BA_Array CacuInterAndSlope(BA_Array dotsAX, BA_Array dotsAY, BA_Array dotsBX, BA_Array dotsBY)
{
	//linesSlope = (dotsAY - dotsBY) / (dotsAX - dotsBX)
	//linesIntercept = dotsAY - linesSlope * dotsAX
	BA_Array linesSlope = dotsAY.Reduc(dotsBY, true).
		Devide(dotsAX.Reduc(dotsBX, true), true);
	BA_Array linesIntercept = dotsAY.Reduc(linesSlope.Mul(dotsAX, true), true);
	// return b, a
	return linesIntercept.Concat(linesSlope, 0);
}
// func for put dots to global var with given dots(x, y)
//cacu and return y=a*x+b (b, a)
BA_Array DrawLinesWithXY(Dots* dotsA, Dots* dotsB, float loopTime, balist<float>& dotsAP, balist<float>& dotsBP)
{
	BA_Array pack = CacuInterAndSlope(dotsA->x, dotsA->y, dotsB->x, dotsB->y);
	for (_ULL idx = 0; idx < dotsA->x.dataLen; idx++)
	{
		dotsAP.Put(floatDup(pba->STmem, 3, dotsA->x.dataF[idx], dotsA->y.dataF[idx], loopTime));
		dotsBP.Put(floatDup(pba->STmem, 3, dotsB->x.dataF[idx], dotsB->y.dataF[idx], loopTime));
	}
	return pack;
}
// func for cacu intersection dots between a group of lines in format of y = a * x + b
// the useful dots exist between the two adjacent lines
// i: idx of sides
Dots CacuIntercetingDots(BA_Array linesIntercept, BA_Array linesSlope, int i)
{
	// create seq, data like 0(padding), b1, b2 or b1, b2, 0(padding)
	_ULL seqSize = linesIntercept.dataLen;
	BA_Array Bi = linesIntercept.Sub(0, seqSize - 1);
	BA_Array Ai = linesSlope.Sub(0, seqSize - 1);
	BA_Array Bj = linesIntercept.Sub(1, seqSize);
	BA_Array Aj = linesSlope.Sub(1, seqSize);
	// x = (Bi - Bj) / (Aj - Ai), i = j - 1
	BA_Array x = Bi.Reduc(Bj, true).Devide(Aj.Reduc(Ai, true), true);
	// y = Ai * x + Bi
	BA_Array y = Ai.Mul(x, true).Add(Bi, true);
	// select data for side i, get idx
	BA_Array xMask = x.Ge(sideSize / 2., true);
	BA_Array yMask = y.Ge(sideSize / 2., true);
	List* idxs = List_Init();
	float xMaskVal = (i <= 1) ? 1.f : 0.f, yMaskVal = (1 <= i && i <= 2) ? 1.f : 0.f;;
	// cacu intersect : A & B
	for (_ULL idx = 0; idx < xMask.dataLen; idx++)
		if (xMask.dataF[idx] == xMaskVal && yMask.dataF[idx] == yMaskVal)
			List_Put(idxs, (void*)TypeDupR(pba->STmem, 1, idx));
	// select data
	BA_Array x_ = BA_Array(BA_Shape(1, idxs->sumque), 0.f);
	BA_Array y_ = BA_Array(BA_Shape(1, idxs->sumque), 0.f);
	LIST_FORS(_ULL, p, idxs)
		if (idxs->now)
		{
			x_.dataF[idxs->now->idx] = x.dataF[*p];
			y_.dataF[idxs->now->idx] = y.dataF[*p];
		}
	// return selected data in format of list(X, Y)
	return Dots(x_, y_);
}