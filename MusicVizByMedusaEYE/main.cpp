
#define USE_SDL2
#include"../../../BA/Cpp/BA_1_2/BA_1_2/BA_1_2.hpp"
#include"../../../BA/Cpp/BA_1_2/BA_1_2/BA_1_2.cpp"
#include"../../../BA/Cpp/BA_1_2/BA_1_2/BA_File.cpp"
#include"../../../BA/Cpp/BA_1_2/BA_1_2/BA_UI.cpp"
#include"../../../BA/Cpp/BA_1_2/BA_1_2/BA_String.cpp"

int main(int argc, char* argv[])
{
	MyBA_Init();

	int* defaultBGCol = intdup(3, 0, 0, 0);
	MyUI* pui = MyUI_Init("MusicVizByMedusaEYE", 800, 800, 0, defaultBGCol);
	pui->win->pre_title = pui->win->pre_win;
	while (pui->pF_PollQuit(pui) == 0)
	{
		pui->pF_Checktitle(pui);
		pui->pF_Update(pui, 1, 1);
	}

	return MyBA_Quit();
}