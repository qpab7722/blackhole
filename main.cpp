#include <stdio.h>
#include <stdlib.h>
#include <Windows.h> 
#include <time.h> 
#include <conio.h>
#include "info.h" 


#define LEFT 75
#define RIGHT 77
#define SPACE 32
/*
#define bool int
#define true 1
#define false 0
*/

COORD PC_pos = { 10,0 };
int curPosX;
int curPosY;
int speed = 30;
int MT_posX;
int MT_posY;
int PCLife = 30;	//PC의 체력
bool attacked = false;	//공격받았는지 알려주는 함수

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

int DetectCollision(int posX, int posY, char MeteoInfo[4][4], char ObInfo[25][25])
{
	int x, y;
	int arrX = posX / 2;
	int arrY = posY;
	for (x = 0; x < 4; x++)
	{
		for (y = 0; y < 4; y++) {
			if (MeteoInfo[y][x] == 1 || MeteoInfo[y][x] == 2)
			{
				if (ObInfo[arrY + y + 1][arrX + (x * 2)] == 1 || ObInfo[arrY + y + 1][arrX + (x * 2)] == 2)
					return 0;
			}

		}
	}
	return 1;
}

void ShowMT(char MeteoInfo[4][4]) {
	int x, y;
	COORD curPos = GetCurrentCursorPos();
	for (y = 0; y<4; y++)
	{
		for (x = 0; x<4; x++)
		{
			SetCurrentCursorPos(curPos.X + (x * 2), curPos.Y + y);
			if (MeteoInfo[y][x] == 1)
				printf("◈");
			if (MeteoInfo[y][x] == 2)
				printf("˚");
		}
	}
	SetCurrentCursorPos(curPos.X, curPos.Y);
}

void DeleteMT(char MeteoInfo[4][4])
{
	int x, y;
	COORD curPos = GetCurrentCursorPos();
	for (y = 0; y<4; y++)
	{
		for (x = 0; x<4; x++)
		{
			SetCurrentCursorPos(curPos.X + (x * 2), curPos.Y + y);
			if (MeteoInfo[y][x] == 1 || MeteoInfo[y][x] == 2)
				printf(" ");
		}
	}
	SetCurrentCursorPos(curPos.X, curPos.Y);
}

void DrawOb(char ObInfo[25][25]) {
	int x, y;

	for (y = 0; y<25; y++)
	{
		for (x = 0; x<25; x++)
		{
			SetCurrentCursorPos((x * 2), y);
			if (ObInfo[y][x] == 1)
				printf("□");
			if (ObInfo[y][x] == 2)
				printf("△");
		}
	}
}

int DrawMeteo()
{
	COORD curPos = GetCurrentCursorPos();

	DeleteMT(MeteoInfo[0]);

	if (curPos.Y == 1) {
		DrawOb(ObInfo[0]);
		return 0;
	}
	if (DetectCollision(curPos.X, curPos.Y + 1, MeteoInfo[0], ObInfo[0]) == 0)
	{
		DrawOb(ObInfo[0]);
	}

	MT_posY -= 1;
	SetCurrentCursorPos(MT_posX, MT_posY);
	ShowMT(MeteoInfo[0]);
	Sleep(200);
	return 1;
}

int Physical(int maxLife)	//체력함수(캐릭터의 최대 체력을 받아서 현재 체력을 리턴)
{
	static int nowLife = maxLife;

	if (nowLife == 0)	//체력이 0일때 game over
	{
		SetCurrentCursorPos(20, curPosY);
		printf("Game Over!\n");
		Sleep(100);
		exit(0);
	}

	else if (attacked)	//아팠을때
		nowLife--;

	attacked = false;	//다시 attacked을 false (원상태)로 돌려준다.
	return nowLife;	//현재 체력을 리턴한다.

}

//PC를 그리는 함수
void drawPC(char PClnfo[4][4])
{
	int x, y;
	COORD curPos = GetCurrentCursorPos();
	for (y = 0; y<4; y++)
	{
		for (x = 0; x<4; x++)
		{
			SetCurrentCursorPos(curPos.X + (x * 2), curPos.Y + y);

			if (PClnfo[y][x] == 1)
			{
				if (y == 1) printf("◎");
				if (y == 2)	printf("▲");
			}
		}
	}
	SetCurrentCursorPos(curPos.X, curPos.Y);
}

//PC를 지우는 함수
void deletePC(char PClnfo[4][4])
{
	int x, y;
	COORD curPos = GetCurrentCursorPos();
	for (y = 0; y<4; y++)
	{
		for (x = 0; x<4; x++)
		{
			SetCurrentCursorPos(curPos.X + (x * 2), curPos.Y + y);

			if (PClnfo[y][x] == 1)
				printf(" ");
		}
	}
	SetCurrentCursorPos(curPos.X, curPos.Y);
}

int isCrash(int posX, int posY, char PCInfo[4][4], char ObInfo[25][25],char MeteoInfo[4][4])	//충돌 함수
{
	int x, y;
	int arrX = (posX) / 2;
	int arrY = posY;
	for (x = 0; x<4; x++)
	{
		for (y = 0; y<4; y++)
		{
			if (PCInfo[y][x] == 1)
			{

				if (ObInfo[arrY + y][arrX + x] == 1)	//벽이랑 부딪혔을때
					return 0;

				if (ObInfo[arrY + y][arrX + x] == 2)	//장애물이랑 부딪혔을때
				{
					attacked = true;
					return 0;
				}

				if (MeteoInfo[arrY + y -1][arrX + (x )] !=0 )	//운석이랑 부딪혔을때
				{
					attacked = true;
					printf("ee");
				}
				
			}
		}
	}
	return 1;
}

int ShiftRight()
{
	if (isCrash(curPosX + 2, curPosY, PCInfo[0], ObInfo[0],MeteoInfo[0]) == 0)
		return 0;
	deletePC(PCInfo[0]);
	DrawOb(ObInfo[0]);
	PC_pos.X += 2;
	SetCurrentCursorPos(PC_pos.X, PC_pos.Y);
	drawPC(PCInfo[0]);
	Sleep(50);
	return 1;

}
int ShiftLeft()
{
	if (isCrash(curPosX - 2, curPosY, PCInfo[0], ObInfo[0], MeteoInfo[0]) == 0)
		return 0;
	deletePC(PCInfo[0]);
	DrawOb(ObInfo[0]);
	PC_pos.X -= 2;
	SetCurrentCursorPos(PC_pos.X, PC_pos.Y);
	drawPC(PCInfo[0]);
	Sleep(50);
	return 1;
}

int Jump()
{
	if (isCrash(curPosX , curPosY - 1, PCInfo[0], ObInfo[0], MeteoInfo[0]) == 0)
		return 0;
	deletePC(PCInfo[0]);
	DrawOb(ObInfo[0]);
	PC_pos.Y -= 1;
	SetCurrentCursorPos(PC_pos.X, PC_pos.Y);
	drawPC(PCInfo[0]);
	Sleep(50);

	return 1;
}

int Gravity_N()
{
	//짧은 간격으로 한칸씩 내려서 두칸 내린다
	if (isCrash(curPosX, curPosY + 1, PCInfo[0], ObInfo[0], MeteoInfo[0]) == 0)
		return 0;
	deletePC(PCInfo[0]);
	DrawOb(ObInfo[0]);
	PC_pos.Y += 1;
	SetCurrentCursorPos(PC_pos.X, PC_pos.Y);
	drawPC(PCInfo[0]);
	Sleep(50);

	//if (isCrash(curPosX, curPosY + 1, PCInfo[0], ObInfo[0], MeteoInfo[0]) == 0)
	//	return 0;
	//deletePC(PCInfo[0]);
	//DrawOb(ObInfo[0]);
	//PC_pos.Y += 1;
	//SetCurrentCursorPos(PC_pos.X, PC_pos.Y);
	//drawPC(PCInfo[0]);
	//Sleep(25);

	return 1;
}

void ProcessKeyInput()
{
	int key;

	for (int i = 0;i<20;i++)
	{
		if (_kbhit() != 0)
		{
			key = _getch();
			switch (key)
			{
			case LEFT:
				ShiftLeft();
				break;
			case RIGHT:
				ShiftRight();
				break;
			case SPACE:
				Jump();
				break;
			}
		}
		Sleep(speed);
	}
}


int main(void)
{

	RemoveCursor();
	DrawOb(ObInfo[0]);

	MT_posY = 25;
	srand((unsigned int)time(NULL));
	MT_posX = (rand() % 5)*2 + 10;

	while (1)
	{
		curPosX = PC_pos.X;
		curPosY = PC_pos.Y;
		SetCurrentCursorPos(curPosX, curPosY);
		drawPC(PCInfo[0]);

		Gravity_N();
		ProcessKeyInput();

	//	SetCurrentCursorPos(60, 0 );
	//	printf("                 ");	//윗줄에 나온거 지워주려고
		SetCurrentCursorPos(60, 0);
		printf("PC 체력: %3d", Physical(PCLife));
		
		SetCurrentCursorPos(MT_posX, MT_posY);

		if (DrawMeteo() == 0) {
			getchar;
			srand((unsigned int)time(NULL));
			MT_posX = (rand() % 5)*2 + 10;
			SetCurrentCursorPos(MT_posX, 25);

		}
	}

	getchar();
	return 0;
}