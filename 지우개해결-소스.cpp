#include <stdio.h>
#include <Windows.h> 
#include <time.h> 
#include "info.h" 

int MT_posX;

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

int DetectCollision(int posX, int posY, char MeteoInfo[4][4],char ObInfo[25][25])
{
	int x, y;
	int arrX = posX / 2;
	int arrY = posY;
	for (x = 0; x < 4; x++)
	{
		for (y = 0; y < 4; y++){
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

void RemoveCursor(void)
{
	CONSOLE_CURSOR_INFO curInfo;
	GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);
	curInfo.bVisible = 0;
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);
}

int DrawMeteo()
{
	COORD curPos = GetCurrentCursorPos();


	DeleteMT(MeteoInfo[0]);

	if (curPos.Y == 1){
		DrawOb(ObInfo[0]);
		return 0;
	}
	if (DetectCollision(curPos.X, curPos.Y + 1, MeteoInfo[0], ObInfo[0]) == 0)
	{
		DrawOb(ObInfo[0]);
	}

	curPos.Y -= 1;
	SetCurrentCursorPos(curPos.X, curPos.Y);
	ShowMT(MeteoInfo[0]);
	Sleep(200);
	return 1;
}

int main(void)
{
	
	RemoveCursor();
	DrawOb(ObInfo[0]);

	SetCurrentCursorPos(MT_posX, 25);
	while (1)
	{
		srand((unsigned int)time(NULL));
		MT_posX = (rand() % 10) + 10;

		if (DrawMeteo() == 0){
			getchar;
			SetCurrentCursorPos(MT_posX, 20);

		}
	}

	getchar();
	return 0;
}