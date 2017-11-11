#include <stdio.h>
#include <Windows.h> 
#include <time.h> 
#include "info.h" 


#define LEFT 75
#define RIGHT 77
#define SPACE 32

COORD PC_pos = { 10,0 };
int curPosX;
int curPosY;
int speed = 30;
int MT_posX;
int MT_posY;

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
				printf("¢Â");
			if (MeteoInfo[y][x] == 2)
				printf("¢ª");
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
				printf("¡à");
			if (ObInfo[y][x] == 2)
				printf("¡â");
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
	DrawOb(ObInfo[0]);
	PC_pos.X += 2;
	SetCurrentCursorPos(PC_pos.X, PC_pos.Y);
	drawPC(PCinfo[0]);
	Sleep(50);
	return 1;

}
int ShiftLeft()
{
	deletePC(PCinfo[0]);
	DrawOb(ObInfo[0]);
	PC_pos.X -= 2;
	SetCurrentCursorPos(PC_pos.X, PC_pos.Y);
	drawPC(PCinfo[0]);
	Sleep(50);
	return 1;
}

int Jump()
{
	deletePC(PCinfo[0]);
	DrawOb(ObInfo[0]);
	PC_pos.Y -= 1;
	SetCurrentCursorPos(PC_pos.X, PC_pos.Y);
	drawPC(PCinfo[0]);
	Sleep(50);

	return 1;
}

int Gravity_N()
{
	deletePC(PCinfo[0]);
	DrawOb(ObInfo[0]);
	PC_pos.Y += 1;
	SetCurrentCursorPos(PC_pos.X, PC_pos.Y);
	drawPC(PCinfo[0]);
	Sleep(25);
	deletePC(PCinfo[0]);
	DrawOb(ObInfo[0]);
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


int main(void)
{

	RemoveCursor();
	DrawOb(ObInfo[0]);

	MT_posY = 25;
	srand((unsigned int)time(NULL));
	MT_posX = (rand() % 10) + 10;

	while (1)
	{
		curPosX = PC_pos.X;
		curPosY = PC_pos.Y;
		SetCurrentCursorPos(curPosX, curPosY);
		drawPC(PCinfo[0]);

		Gravity_N();
		ProcessKeyInput();

		
		SetCurrentCursorPos(MT_posX, MT_posY);

		if (DrawMeteo() == 0) {
			getchar;
			srand((unsigned int)time(NULL));
			MT_posX = (rand() % 10) + 10;
			SetCurrentCursorPos(MT_posX, 25);

		}
	}

	getchar();
	return 0;
}