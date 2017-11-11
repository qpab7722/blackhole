#include<stdio.h>
#include <windows.h>
#include "info.h"

#define LEFT 75
#define RIGHT 77
#define SPACE 32

COORD PC_pos = {10,10};
int curPosX ;
int curPosY ;
int speed = 30;

void RemoveCursor(void)
{
	CONSOLE_CURSOR_INFO curInfo;
	GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);
	curInfo.bVisible = 0;
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);
}

void SetCurrentCursorPos(int x, int y)
{
	COORD pos = { x, y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
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

void drawPC(char PCinfo[4][4])
{
	int x, y;
	COORD curPos = GetCurrentCursorPos();	
	for (y = 0; y<4; y++)
	{
		for (x = 0; x<4; x++)
		{
			SetCurrentCursorPos(curPos.X + (x * 2), curPos.Y + y);

			if (PCinfo[y][x] == 1)
			{
				if (y == 1) printf("¡Ý");
				if (y == 2)	printf("¡â");
			}
		}
	}
	SetCurrentCursorPos(curPos.X, curPos.Y);
}

void deletePC(char PCinfo[4][4])
{
	int x, y;
	COORD curPos = GetCurrentCursorPos();
	for (y = 0; y<4; y++)
	{
		for (x = 0; x<4; x++)
		{
			SetCurrentCursorPos(curPos.X + (x * 2), curPos.Y + y);

			if (PCinfo[y][x] == 1)
				printf(" ");
		}
	}
	SetCurrentCursorPos(curPos.X, curPos.Y);
}

int ShiftRight()
{
	deletePC(PCinfo[0]);
	PC_pos.X += 2;
	SetCurrentCursorPos(PC_pos.X, PC_pos.Y);
	drawPC(PCinfo[0]);
	Sleep(50);
	return 1;
	
}
int ShiftLeft()
{	
	deletePC(PCinfo[0]);
	PC_pos.X -= 2;
	SetCurrentCursorPos(PC_pos.X, PC_pos.Y);
	drawPC(PCinfo[0]);
	Sleep(50);
	return 1;	
}

int Jump()
{
	deletePC(PCinfo[0]);
	PC_pos.Y -= 1;
	SetCurrentCursorPos(PC_pos.X, PC_pos.Y);
	drawPC(PCinfo[0]);
	Sleep(50);
	
	return 1;
}

int Gravity_N()
{
	deletePC(PCinfo[0]);
	PC_pos.Y += 1;
	SetCurrentCursorPos(PC_pos.X, PC_pos.Y);
	drawPC(PCinfo[0]);
	Sleep(25);
	deletePC(PCinfo[0]);
	PC_pos.Y += 1;
	SetCurrentCursorPos(PC_pos.X, PC_pos.Y);
	drawPC(PCinfo[0]);
	Sleep(25);


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


void main()
{
	RemoveCursor();
	
	while (1)
	{		
		curPosX = PC_pos.X;
		curPosY = PC_pos.Y;
		SetCurrentCursorPos(curPosX, curPosY);
		drawPC(PCinfo[0]);

		Gravity_N();
		ProcessKeyInput();		
	}

	getchar();
}