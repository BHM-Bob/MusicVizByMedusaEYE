
#include"../../../BA/Cpp/BA_1_2/BA_1_2/BA_1_2.hpp"
#include"../../../BA/Cpp/BA_1_2/BA_1_2/BA_1_2.cpp"
#include"../../../BA/Cpp/BA_1_2/BA_1_2/BA_File.cpp"
#include"../../../BA/Cpp/BA_1_2/BA_1_2/BA_UI.cpp"
#include"../../../BA/Cpp/BA_1_2/BA_1_2/BA_String.cpp"
#include"../../../BA/Cpp/BA_1_2/BA_1_2/BA_Math.cpp"

#include"SD.h"
#include"cacu.h"

// func for drawing a line in window using SDL_RenderDrawPoint
void DrawLineInWindow(float* dot1, float* dot2, MyUI* pui, int* col, float* coli, float* dColi)
{
	float winH = pui->win->pre_win->h;
	float x1 = dot1[0], y1 = winH - dot1[1], x2 = dot2[0], y2 = winH - dot2[1];
	if (x1 == x2 && y1 == y2)
		return;// shortcut quit
	float signDX = x2 > x1 ? 1. : -1., signDY = y2 > y1 ? 1. : -1.;
	float absDX = x2 > x1 ? x2 - x1 : x1 - x2, absDY = y2 > y1 ? y2 - y1 : y1 - y2;
	float ddx = (x1 == x2) ? 0. : (absDX >= absDY ? signDX : signDX * absDX / absDY);
	float ddy = (y1 == y2) ? 0. : (absDY >= absDX ? signDY : signDY * absDY / absDX);
	float steps = (ddx == signDX) ? absDX : absDY;
	int x = 0, y = 0;
	for (float dx = 0, dy = 0, s = 0; s < steps; dx += ddx, dy += ddy, s++)
	{
		x = (int)(x1 + dx), y = (int)(y1 + dy);
		col = ProduceRainbowCol(col, coli, *dColi);
		SDL_SetRenderDrawColor(pui->win->rend, col[0], col[1], col[2], 255);
		SDL_RenderDrawPoint(pui->win->rend, x, y);
	}
}

int main(int argc, char* argv[])
{
	MyBA_Init();
	balist<float> dotsAP = balist<float>();
	balist<float> dotsBP = balist<float>();
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
	balist<Dots> sideDots = balist<Dots>(&d1, &d2, &d3, &d4, &d1, NULL);
	// draw first loop
	Dots intercetingDotsPack = d1;
	for (int i = 0; i < 4; i++)
	{
		BA_Array pack = DrawLinesWithXY(sideDots[i], sideDots[i+1], 0.f, dotsAP, dotsBP);
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
		sideDots.Destroy();
		sideDots = balist<Dots>(&d1, &d2, &d3, &d4, &d1, NULL);
		for (int i = 0; i < 4; i++)
		{
			BA_Array pack = DrawLinesWithXY(sideDots[i], sideDots[i + 1], loopTime, dotsAP, dotsBP);
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
	MyUI* pui = MyUI_Init("MusicVizByMedusaEYE",
		(_ULL)sideSize, (_ULL)sideSize, 0, NULL);
	pui->win->pre_title = pui->win->pre_win;
	int* col = TypeDupR(NULL, 3, 0, 0, 0);
	float* coli = floatDup(NULL, 2, (float)rand() / 9.f, 0.0009f);
	// GUI Loop
	while (pui->pF_PollQuit(pui) == 0)
	{
		for (float* p1 = dotsAP.Copy(), *p2 = dotsBP.Copy();
			p1 != NULL && pui->pF_PollQuit(pui) == 0;
			p1 = dotsAP.Copy(), p2 = dotsBP.Copy())
			if (p1[2] <= 2 && (p1[0] != 0. || p1[1] != 0.) && (p2[0] != 0. || p2[1] != 0.))
				DrawLineInWindow(p1, p2, pui, col, coli, coli + 1);
		SDL_RenderPresent(pui->win->rend);
		pui->pF_Checktitle(pui);
		pui->pF_Update(pui, 0, 0);
	}
	return MyBA_Quit();
}