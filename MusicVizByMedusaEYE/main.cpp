
#include"../../../BA/Cpp/BA_1_2/BA_1_2/BA_1_2.hpp"
#include"../../../BA/Cpp/BA_1_2/BA_1_2/BA_1_2.cpp"
#include"../../../BA/Cpp/BA_1_2/BA_1_2/BA_File.cpp"
#include"../../../BA/Cpp/BA_1_2/BA_1_2/BA_UI.cpp"
#include"../../../BA/Cpp/BA_1_2/BA_1_2/BA_String.cpp"
#include"../../../BA/Cpp/BA_1_2/BA_1_2/BA_Math.cpp"

// global var
List* dotsAP = List_Init(), * dotsBP = List_Init();
//set global settings
float sideSize = 800.f, interval = 10.f;
_ULL loopTimes = 3;
// useful dataStruct
typedef struct Dots Dots;
struct Dots
{
	BA_Array x;
	BA_Array y;
};

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
BA_Array DrawLinesWithXY(Dots* dotsA, Dots* dotsB, float loopTime)
{
	BA_Array pack = CacuInterAndSlope(dotsA->x, dotsA->y, dotsB->x, dotsB->y);
	for (_ULL idx = 0; idx < dotsA->x.dataLen; idx++)
	{
		List_Put(dotsAP, (void*)floatdup(3, dotsA->x.dataF[idx], dotsA->y.dataF[idx], loopTime));
		List_Put(dotsBP, (void*)floatdup(3, dotsB->x.dataF[idx], dotsB->y.dataF[idx], loopTime));
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
	//if (x.dataF[0] == 0 && y.dataF[0] == 0)
	//{
	//	Bi.Str();
	//	Ai.Str();
	//	Bj.Str();
	//	Aj.Str();
	//	x.Str();
	//	y.Str();
	//}
	// select data for side i, get idx
	BA_Array xMask = x.Ge(sideSize / 2., true);
	BA_Array yMask = y.Ge(sideSize / 2., true);
	List* idxs = List_Init();
	float xMaskVal = (i <= 1) ? 1.f : 0.f, yMaskVal = (1 <= i && i <= 2) ? 1.f : 0.f;;
	// cacu intersect : A & B
	for (_ULL idx = 0; idx < xMask.dataLen; idx++)
		if (xMask.dataF[idx] == xMaskVal && yMask.dataF[idx] == yMaskVal)
			List_Put(idxs, (void*)ULLdup(1, idx));
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

int main(int argc, char* argv[])
{
	// TODO: optimize MyUI mem control,
	// add destroy func or link to MyBA_Quit
	MyBA_Init();
	// usefull var for fast understanding
	BA_Shape maxSideDots = BA_Shape(1, (_ULL)(sideSize / interval - 1));
	BA_Array fromInt2S = BA_Array(maxSideDots, 0.f).Seq(interval, interval);
	BA_Array fromS2Int = BA_Array(maxSideDots, 0.f).Seq(sideSize - interval, -interval);
	BA_Array all0 = BA_Array(maxSideDots, 0.f);
	BA_Array allS = BA_Array(maxSideDots, sideSize);
	// first loop dots
	Dots d1 = Dots(fromInt2S, all0);
	Dots d2 = Dots(allS, fromInt2S);
	Dots d3 = Dots(fromS2Int, allS);
	Dots d4 = Dots(all0, fromS2Int);
	List* sideDots = List_Gather((void*)&d1, (void*)&d2, (void*)&d3,
		(void*)&d4, (void*)&d1, NULL);
	// draw first loop
	Dots intercetingDotsPack = d1;
	for (int i = 0; i < 4; i++)
	{
		BA_Array pack = DrawLinesWithXY((Dots*)List_Index(sideDots, i),
			(Dots*)List_Index(sideDots, i + 1), 0.f);
		Dots dots = CacuIntercetingDots(pack.Sub(0, (_ULL)(pack.dataLen / 2)),
			pack.Sub((_ULL)(pack.dataLen / 2), pack.dataLen), i);
		if (i == 0)
		{
			intercetingDotsPack = dots;
		}
		else
		{
			intercetingDotsPack.x = intercetingDotsPack.x.Concat(dots.x, 0);
			intercetingDotsPack.y = intercetingDotsPack.y.Concat(dots.y, 0);
		}
	}
	//draw left loops
	for (_ULL loopTime = 1, sideLen = 0; loopTime < loopTimes; loopTime++)
	{
		sideLen = (_ULL)(intercetingDotsPack.x.dataLen / 4);
		d1 = Dots(intercetingDotsPack.x.Sub(0, sideLen), intercetingDotsPack.y.Sub(0, sideLen));
		d2 = Dots(intercetingDotsPack.x.Sub(sideLen, 2 * sideLen), intercetingDotsPack.y.Sub(sideLen, 2 * sideLen));
		d3 = Dots(intercetingDotsPack.x.Sub(2 * sideLen, 3 * sideLen), intercetingDotsPack.y.Sub(2 * sideLen, 3 * sideLen));
		d4 = Dots(intercetingDotsPack.x.Sub(3 * sideLen, 4 * sideLen), intercetingDotsPack.y.Sub(3 * sideLen, 4 * sideLen));
		sideDots = List_Gather((void*)&d1, (void*)&d2, (void*)&d3,
			(void*)&d4, (void*)&d1, NULL);
		for (int i = 0; i < 4; i++)
		{
			BA_Array pack = DrawLinesWithXY((Dots*)List_Index(sideDots, i),
				(Dots*)List_Index(sideDots, i + 1), (float)loopTime);
			Dots dots = CacuIntercetingDots(pack.Sub(0, (_ULL)(pack.dataLen / 2)),
				pack.Sub((_ULL)(pack.dataLen / 2), pack.dataLen), i);
			if (i == 0)
			{
				intercetingDotsPack = dots;
			}
			else
			{
				intercetingDotsPack.x = intercetingDotsPack.x.Concat(dots.x, 0);
				intercetingDotsPack.y = intercetingDotsPack.y.Concat(dots.y, 0);
			}
		}
	}
	// init a NULL window
	int* defaultBGCol = intdup(3, 0, 0, 0);
	MyUI* pui = MyUI_Init("MusicVizByMedusaEYE",
		(_ULL)sideSize, (_ULL)sideSize, 0, defaultBGCol);
	pui->win->pre_title = pui->win->pre_win;
	int* col = intdup(3, 0, 0, 0), x1,y1,x2,y2;
	float* coli = floatdup(1, (float)rand()/9.f);
	// draw plot
	//SDL_Surface* pSur_paint = SDL_CreateRGBSurface(0, 1, 1, 32, 0, 0, 0, 0);
	//SDL_FillRect(pSur_paint, NULL, SDL_MapRGB(pSur_paint->format, 255, 255, 255));
	//SDL_Texture* pTex_paint = SDL_CreateTextureFromSurface(pui->win->rend, pSur_paint);
	//SDL_Rect re_paint = { 0,0, 1 ,  1 };
	// GUI Loop
	while (pui->pF_PollQuit(pui) == 0)
	{
		for (float* p1 = (float*)List_Copy(dotsAP), *p2 = (float*)List_Copy(dotsBP);
			p1 != NULL && pui->pF_PollQuit(pui) == 0;
			p1 = (float*)List_Copy(dotsAP), p2 = (float*)List_Copy(dotsBP))
		{
			x1 = int(p1[0]), y1 = int(p1[1]), x2 = int(p2[0]), y2 = int(p2[1]);
			if ( x1 + y1 != 0 && x2 + y2 != 0 && p1[2]>=0)
			{
				col = ProduceRainbowCol(col, coli);
				SDL_SetRenderDrawColor(pui->win->rend, col[0], col[1], col[2], 255);
				//printf("%4d | %4d - %4d | %4d  -  %4d %4d %4d\n", x1, y1, x2, y2, col[0], col[1], col[2]);
				SDL_RenderDrawLine(pui->win->rend, x1, y1, x2, y2);
				SDL_Delay(2);
				SDL_RenderPresent(pui->win->rend);
			}
		}
		pui->pF_Checktitle(pui);
		pui->pF_Update(pui, 0, 0);
	}
	return MyBA_Quit();
}