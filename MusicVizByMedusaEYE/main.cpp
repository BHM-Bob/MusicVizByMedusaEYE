
#include"../../../BA/Cpp/BA_1_2/BA_1_2/BA_Base.hpp"
#include"../../../BA/Cpp/BA_1_2/BA_1_2/BA_1_2.cpp"
#include"../../../BA/Cpp/BA_1_2/BA_1_2/BA_CMD.cpp"
#include"../../../BA/Cpp/BA_1_2/BA_1_2/BA_Mem.cpp"
#include"../../../BA/Cpp/BA_1_2/BA_1_2/BA_File.cpp"
#include"../../../BA/Cpp/BA_1_2/BA_1_2/BA_String.cpp"
#include"../../../BA/Cpp/BA_1_2/BA_1_2/BA_Math.hpp"

#include"../../../BA/Cpp/BA_1_2/BA_1_2/BA_GUI.hpp"
#include"../../../BA/Cpp/BA_1_2/BA_1_2/BA_GUI.cpp"

//set global settings
const float sideSize = 1000.f, interval = 10.f;
const _ULL loopTimes = 3;

typedef std::vector<ba::tensor<float>> tensors;

// func for cacu y = a * x + b from(x1, y1) and (x2, y2)
tensors CacuInterAndSlope(ba::tensor<float> dotsAX, ba::tensor<float> dotsAY, ba::tensor<float> dotsBX, ba::tensor<float> dotsBY)
{
	ba::tensor<float> linesSlope = (dotsAY - dotsBY) / (dotsAX - dotsBX);
	ba::tensor<float> linesIntercept = dotsAY - linesSlope * dotsAX;
	// return b, a
	return tensors{ linesIntercept, linesSlope };
}
// func for put dots to global var with given dots(x, y)
//cacu and return y=a*x+b (b, a)
tensors DrawLinesWithXY(tensors dotsA, tensors dotsB,
	float loopTime, balist<float>& dotsAP, balist<float>& dotsBP)
{
	tensors pack = CacuInterAndSlope(dotsA[0], dotsA[1], dotsB[0], dotsB[1]);
	ba::tensor<float> linesIntercept = pack[0], linesSlope = pack[1];
	for (_LL idx = 0; idx < dotsA[0].len; idx++)
	{
		dotsAP.Put(TypeDupR(pba->STmem, 3, dotsA[0].data[idx], dotsA[1].data[idx], loopTime));
		dotsBP.Put(TypeDupR(pba->STmem, 3, dotsB[0].data[idx], dotsB[0].data[idx], loopTime));
	}
	return pack;
}
// func for cacu intersection dots between a group of lines in format of y = a * x + b
// the useful dots exist between the two adjacent lines
// i: idx of sides
tensors CacuIntercetingDots(ba::tensor<float> linesIntercept, ba::tensor<float> linesSlope, int i)
{
	// create seq, data like 0(padding), b1, b2 or b1, b2, 0(padding)
	_LL seqSize = linesIntercept.shape[0];
	ba::tensor<float> Bi = linesIntercept.sub(0, seqSize - 1);
	ba::tensor<float> Ai = linesSlope.sub(0, seqSize - 1);
	ba::tensor<float> Bj = linesIntercept.sub(1, seqSize);
	ba::tensor<float> Aj = linesSlope.sub(1, seqSize);
	// x = (Bi - Bj) / (Aj - Ai), i = j - 1
	ba::tensor<float> x = (Bi - Bj) / (Aj - Ai);
	// y = Ai * x + Bi
	ba::tensor<float> y = Ai * x + Bi;
	// select data for side i, get idx
	bool xMaskVal = (i <= 1) ? 1.f : 0.f, yMaskVal = (1 <= i && i <= 2) ? 1.f : 0.f;
	ba::tensor<float> xMask = x.map([=](float r) {
		return (r >= (sideSize / 2.f)) == xMaskVal ? 1.f : 0.f; });
	ba::tensor<float> yMask = y.map([=](float r) {
		return (r >= (sideSize / 2.f)) == yMaskVal ? 1.f : 0.f; });
	List* idxs = List_Init();
	ba::tensor<float> mask = xMask * yMask;
	// return selected data in format of list(X, Y)
	return tensors{ x.masksub(mask), y.masksub(mask) };
}
// func for drawing a line in window using SDL_RenderDrawPoint
void DrawLineInWindow(float* dot1, float* dot2, ba::ui::colorSur* cs)
{
	float winH = (float)cs->re.h;
	float x1 = dot1[0], y1 = winH - dot1[1], x2 = dot2[0], y2 = winH - dot2[1];
	if (x1 == x2 && y1 == y2)
		return;// shortcut quit
	float signDX = x2 > x1 ? 1.f : -1.f, signDY = y2 > y1 ? 1.f : -1.f;
	float absDX = x2 > x1 ? x2 - x1 : x1 - x2, absDY = y2 > y1 ? y2 - y1 : y1 - y2;
	float ddx = (x1 == x2) ? 0.f : (absDX >= absDY ? signDX : signDX * absDX / absDY);
	float ddy = (y1 == y2) ? 0.f : (absDY >= absDX ? signDY : signDY * absDY / absDX);
	float steps = (ddx == signDX) ? absDX : absDY;
	int x = 0, y = 0;
	for (float dx = 0.f, dy = 0.f, s = 0.f; s < steps; dx += ddx, dy += ddy, s++)
	{
		x = (int)(x1 + dx), y = (int)(y1 + dy);
		if(0<=x && x<cs->re.w && 0 <= y && y < cs->re.h)
			cs->mask[y][x] = 1;
	}
}

int main(int argc, char* argv[])
{
	MyBA_Init();
	balist<float> dotsAP = balist<float>();
	balist<float> dotsBP = balist<float>();
	// usefull var for fast understanding
	ba::tensor<float> maxSideDots({ (_LL)(sideSize / interval) - 1 });
	ba::tensor<float> fromInt2S = ba::tensor<float>({ (_LL)(sideSize / interval) - 1 }).\
		selfmap([&](float* r, _LL i) { *r = interval * (i + 1); });//.Seq(interval, interval);
	ba::tensor<float> fromS2Int = ba::tensor<float>({ (_LL)(sideSize / interval) - 1 }).\
		selfmap([&](float* r, _LL i) { *r = sideSize - interval * (i + 1); });//.Seq(sideSize - interval, -interval);
	ba::tensor<float> all0({ (_LL)(sideSize / interval) - 1 });
	ba::tensor<float> allS({ (_LL)(sideSize / interval) - 1 }, sideSize);
	// draw first loop
	tensors pack, dots;
	std::vector<tensors> intercetingDotsPack(4), sideDots;
	intercetingDotsPack[0] = { fromInt2S, all0 };
	intercetingDotsPack[1] = { allS, fromInt2S };
	intercetingDotsPack[2] = { fromS2Int, allS };
	intercetingDotsPack[3] = { all0, fromS2Int };
	for (_ULL loopTime = 0, sideLen = 0; loopTime < loopTimes; loopTime++)
	{
		sideDots = { intercetingDotsPack[0], intercetingDotsPack[1], intercetingDotsPack[2],
			intercetingDotsPack[3], intercetingDotsPack[0] };
		intercetingDotsPack.clear();
		for (int i = 0; i < 4; i++)
		{
			pack = DrawLinesWithXY(sideDots[i], sideDots[i + 1], 0.f, dotsAP, dotsBP);
			dots = CacuIntercetingDots(pack[0], pack[1], i);
			intercetingDotsPack.push_back(dots);
		}
	}
	// init a NULL window
	ba::ui::QUI ui("MusicVizByMedusaEYE", (_ULL)sideSize, (_ULL)sideSize, 0, NULL);
	ba::ui::colorSur cs(ui.activeWin, NULL, ui.activeWin->re, true);
	for (float* p1 = dotsAP.Copy(), *p2 = dotsBP.Copy();
		p1 != NULL;
		p1 = dotsAP.Copy(), p2 = dotsBP.Copy())
		if (p1[2] <= 2 && (p1[0] >= 0. || p1[1] >= 0.) && (p2[0] >= 0. || p2[1] >= 0.))
			DrawLineInWindow(p1, p2, &cs);
	ui.addOtherTex("cst", NULL, &cs.re);
	// GUI Loop
	while (ui.pollQuit() == 0)
	{
		ui.updateOtherTex("cst", cs.getTex());
		ui.update(NULL, 1, 1);
	}
	return MyBA_Quit();
}