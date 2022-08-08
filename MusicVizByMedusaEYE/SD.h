#ifndef SD_H
#define SD_H

#include"../../../BA/Cpp/BA_1_2/BA_1_2/BA_1_2.hpp"


//set global settings
const float sideSize = 1000.f, interval = 10.f;
const _ULL loopTimes = 3;

typedef struct Dots Dots;
struct Dots
{
	BA_Array x;
	BA_Array y;
};

typedef struct SD_GameEvent SD_GameEvent;
struct SD_GameEvent
{
	bool alive;//是否有效
	clock_t clockTab;//clock()
	clock_t contime;
	clock_t leftime;
	_ULL x;
	_ULL y;
	int fx;
	char ch;//for print
	float xSpeed;
	float ySpeed;
	float attack;
	float armor;
};

#endif