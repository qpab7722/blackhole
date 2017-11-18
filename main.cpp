#include <stdio.h>
#include <stdlib.h>
#include <Windows.h> 
#include <time.h> 
#include <conio.h>


#define LEFT 75
#define RIGHT 77
#define SPACE 32
/*
#define bool int
#define true 1
#define false 0
*/

#define GBOARD_HEIGHT  29
#define GBOARD_WIDTH  25


COORD PC_pos = { 10, 0 };
COORD MT_pos = { 0, 0 };
int curPosX;
int curPosY;
int speed = 30;
int check = 0; // 스위치후 delete

int PCLife = 30;   //PC의 체력
bool attacked = false;   //공격받았는지 알려주는 함수
bool Switch_N = false; //스위치 충돌 알려주는 함수

int GBInfo_N[GBOARD_HEIGHT][GBOARD_WIDTH];
int ObTime;
int Check_Ob = 0;
int Ran;

void RemoveCursor(void)
{
	CONSOLE_CURSOR_INFO curInfo;
	GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);
	curInfo.bVisible = 0;
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);
}

COORD GetCurrentCursorPos(void)
{
	COORD curPoint;
	CONSOLE_SCREEN_BUFFER_INFO curInfo;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);
	curPoint.X = curInfo.dwCursorPosition.X;
	curPoint.Y = curInfo.dwCursorPosition.Y;
	return curPoint;
}

void SetCurrentCursorPos(int x, int y)
{
	COORD pos = { x, y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

void DrawOb() {
	int x, y;

	for (y = 0; y<29; y++)
	{
		for (x = 0; x<25; x++)
		{
			SetCurrentCursorPos((x * 2), y);
			if (GBInfo_N[y][x] == 1)
				printf("■");
			else
				printf("　");

		}
	}
}

void UpOB()
{
	int line, x, y;
	for (y = 0; y < GBOARD_HEIGHT; y++)
	{
		memcpy(&GBInfo_N[y][1], &GBInfo_N[y + 1][1], GBOARD_WIDTH*sizeof(int));//내장함수,복사할 떄 많이들 사용
	}

	
}

void MakeOb()
{
	srand((unsigned int)time(NULL));
	ObTime = (rand() % 6) * 2;
	Ran = (rand() % 8) * 2;

	for (int x = ObTime; x < ObTime + Ran; x++)
		GBInfo_N[28][x] = 1;
}

int main(void)
{
	for (int x = 0; x < GBOARD_WIDTH; x++)
		GBInfo_N[28][x] = 1;

	RemoveCursor();
	DrawOb();

	while (1)
	{
		UpOB();
		Sleep(100);
		DrawOb();
		Check_Ob++;

		if (Check_Ob%6 == 0)
			MakeOb();
	}

	getchar();
	return 0;
}